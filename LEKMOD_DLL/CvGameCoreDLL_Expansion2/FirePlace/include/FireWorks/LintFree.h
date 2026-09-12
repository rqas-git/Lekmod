#ifndef _INCLUDED_LintFree_H
#define _INCLUDED_LintFree_H
































#ifdef _MSC_VER


#define LINT_WARNINGS \
	4100                                     \
	4130                                                         \
	4189                                                        \
	4239                                  \
	4238                                                                \
	4505                                                    \
	4512                                                  \
	4702                        \
	4706


#define LINT_CODE_ANALYSIS \
	6001                                  \
	6011                                    \
	6246                                                                                                                                                            \
	6262                                                                                                                  \
	6302                              \
	6385                                                                                                                             \
	6386


#define LINT_SUPPRESSIONS \
	6255                                                                                                        \
	6326




#	pragma warning ( 1 : LINT_WARNINGS )
#	ifndef FINAL_RELEASE
#		ifndef LINT_WARNINGS_ONLY
#			pragma warning ( error : LINT_WARNINGS LINT_CODE_ANALYSIS )
#		endif
#	endif
#	pragma warning ( disable : LINT_SUPPRESSIONS )
#	ifdef LINT_EXTRA_SUPPRESSIONS
#		pragma warning ( disable : LINT_EXTRA_SUPPRESSIONS )
#	endif
#endif

#endif
