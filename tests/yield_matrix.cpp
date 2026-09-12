// Compile the production statement cache, loader and allocator against SQLite.
#include <cassert>
#include <cstdio>
#include <map>
#include <string>
#include <sqlite3.h>
#define CvAssertMsg(condition, message) assert(condition)
#define FNEW(object, domain, flags) new object

template<size_t N>
int sprintf_s(char (&buffer)[N], const char* format, const char* value) {
    return std::snprintf(buffer, N, format, value);
}

namespace Database {
class Results {
    sqlite3_stmt* statement;
public:
    Results() : statement(NULL) {}
    Results(sqlite3* db, const char* sql) : statement(NULL) { assert(Prepare(db, sql)); }
    bool Prepare(sqlite3* db, const char* sql) { return sqlite3_prepare_v2(db, sql, -1, &statement, NULL) == SQLITE_OK; }
    ~Results() { sqlite3_finalize(statement); }
    void Bind(int index, const char* value) { assert(sqlite3_bind_text(statement, index, value, -1, SQLITE_TRANSIENT) == SQLITE_OK); }
    bool Step() { int result = sqlite3_step(statement); assert(result == SQLITE_ROW || result == SQLITE_DONE); return result == SQLITE_ROW; }
    int GetInt(int index) { return sqlite3_column_int(statement, index); }
    void Reset() { assert(sqlite3_reset(statement) == SQLITE_OK); }
};
}

struct DatabaseHost {
    sqlite3* connection;
    int prepared;
    std::map<std::string, int> preparesBySQL;
    bool Execute(Database::Results& result, const char* sql) {
        ++prepared;
        ++preparesBySQL[sql];
        return result.Prepare(connection, sql);
    }
} DB;

class CvDatabaseUtility {
    typedef std::map<std::string, Database::Results*> ResultsMap;
    ResultsMap m_storedResults;
public:
    ~CvDatabaseUtility() { ClearResults(); }
    void ClearResults();
    int MaxRows(const char* table);
    Database::Results* GetResults(const std::string& key);
    Database::Results* PrepareResults(const std::string& key, const char* sql);
    Database::Results* GetOrPrepareResults(const std::string& key, const char* sql);
    bool Initialize2DArray(int**& array, const char* rows, const char* columns, int value = 0);
    void SetYieldMatrix(int**& array, const char* dimension, const char* key, const char* sql, const char* owner);
};

#include "production_matrix.h"
#include "matrix_cases.h"

void checkStatementCache() {
    CvDatabaseUtility utility;
    DB.prepared = 0;
    Database::Results* first = utility.GetOrPrepareResults("first", "SELECT ? + 10");
    assert(first && DB.prepared == 1);
    first->Bind(1, "7");
    assert(first->Step() && first->GetInt(0) == 17);

    // Reuse resets an unfinished statement, retains bindings, and ignores new SQL.
    assert(utility.GetOrPrepareResults("first", "invalid SQL") == first);
    assert(DB.prepared == 1);
    assert(first->Step() && first->GetInt(0) == 17);
    first = utility.GetOrPrepareResults("first", "invalid SQL");
    first->Bind(1, "5");
    assert(first->Step() && first->GetInt(0) == 15);
    assert(!first->Step());
    assert(utility.GetOrPrepareResults("second", "SELECT ? + 10") != first);
    assert(DB.prepared == 2);

    // Failed preparation is not cached, so a corrected query can retry the key.
    assert(utility.GetOrPrepareResults("retry", "invalid SQL") == NULL);
    assert(utility.GetResults("retry") == NULL && DB.prepared == 3);
    Database::Results* retry = utility.GetOrPrepareResults("retry", "SELECT 42");
    assert(retry && retry->Step() && retry->GetInt(0) == 42 && DB.prepared == 4);
    utility.ClearResults();
    assert(utility.GetResults("first") == NULL);
    first = utility.GetOrPrepareResults("first", "SELECT 9");
    assert(first && first->Step() && first->GetInt(0) == 9 && DB.prepared == 5);
}

void execute(sqlite3* db, const char* sql) {
    char* error = NULL;
    if (sqlite3_exec(db, sql, NULL, NULL, &error) != SQLITE_OK) {
        std::fprintf(stderr, "%s\n", error);
        assert(false);
    }
}

void checkSizingStatements() {
    CvDatabaseUtility utility;
    const int before = DB.prepared;
    execute(DB.connection, "CREATE TABLE Sizing (ID INTEGER PRIMARY KEY)");
    // Retain Civ's existing empty-table convention: NULL max(rowid) becomes 1.
    assert(utility.MaxRows("Sizing") == 1);
    execute(DB.connection, "INSERT INTO Sizing VALUES (0),(1),(100)");
    assert(utility.MaxRows("Sizing") == 101);
    execute(DB.connection, "DELETE FROM Sizing WHERE ID = 100");
    assert(utility.MaxRows("Sizing") == 2);
    assert(DB.prepared == before + 1);
    // A prepared SQLite statement also survives a schema rebuild between calls.
    execute(DB.connection, "DROP TABLE Sizing; CREATE TABLE Sizing (ID INTEGER PRIMARY KEY); INSERT INTO Sizing VALUES (7)");
    assert(utility.MaxRows("Sizing") == 8);
    assert(DB.prepared == before + 1);
    assert(utility.MaxRows("LaterTable") == 0);
    execute(DB.connection, "CREATE TABLE LaterTable (ID INTEGER PRIMARY KEY); INSERT INTO LaterTable VALUES (20)");
    assert(utility.MaxRows("LaterTable") == 21);
    utility.ClearResults();
    const int cleared = DB.prepared;
    assert(utility.MaxRows("Sizing") == 8 && DB.prepared == cleared + 1);
}

int main() {
    sqlite3* db = NULL;
    assert(sqlite3_open(":memory:", &db) == SQLITE_OK);
    DB.connection = db;
    checkStatementCache();
    checkSizingStatements();
    DB.prepared = 0;
    DB.preparesBySQL.clear();
    const char* dimensions[] = {"Eras", "Technologies", "Resources", "Buildings", "Improvements", "ResourceClasses",
                                "BuildingClasses", "Features", "Terrains"};
    for (unsigned int i = 0; i < sizeof(dimensions) / sizeof(dimensions[0]); ++i) {
        execute(db, (std::string("CREATE TABLE ") + dimensions[i] + " (ID INTEGER PRIMARY KEY, Type TEXT)").c_str());
        execute(db, (std::string("INSERT INTO ") + dimensions[i] + " VALUES (0,'row0'),(1,'row1'),(4,'row4')").c_str());
    }
    execute(db, "CREATE TABLE Yields (ID INTEGER PRIMARY KEY, Type TEXT); INSERT INTO Yields VALUES (0,'y0'),(1,'y1'),(3,'y3')");
    {
        CvDatabaseUtility utility;
        for (unsigned int i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
            const MatrixCase& test = cases[i];
            execute(db, test.fixture);
            const char* owners[] = {"ownerA", "ownerB", "no rows"};
            for (int owner = 0; owner < 3; ++owner) {
                int** matrix = NULL;
                utility.SetYieldMatrix(matrix, test.dimension, test.key, test.query, owners[owner]);
                for (int row = 0; row < 5; ++row) {
                    for (int yield = 0; yield < 4; ++yield) {
                        int expected = owner == 0 && row == 0 && yield == 1 ? 9 :
                            owner == 0 && row == 4 && yield == 3 ? -4 :
                            owner == 1 && row == 1 && yield == 0 ? 11 : 0;
                        assert(matrix[row][yield] == expected);
                    }
                }
                delete[] reinterpret_cast<unsigned char*>(matrix);
            }
            assert(DB.preparesBySQL[test.query] == 1);
            assert(DB.preparesBySQL[std::string("SELECT max(rowid) from ") + test.dimension] == 1);
            assert(DB.preparesBySQL["SELECT max(rowid) from Yields"] == 1);
        }
    }
    assert(sqlite3_close(db) == SQLITE_OK);
    std::puts("Matrix loading, cache rebinds, and sizing reuse across data/schema changes passed");
}
