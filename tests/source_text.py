import re

CPP = re.compile(r'R"([^ ()\\\t\r\n]{0,16})\(.*?\)\1"|"(?:\\[\s\S]|[^"\\])*"|\'(?:\\[\s\S]|[^\'\\])*\'|//(?:\\\r?\n|[^\r\n])*|/\*[\s\S]*?\*/', re.S)
LUA = re.compile(r'--\[(=*)\[[\s\S]*?\]\1\]|--[^\r\n]*|\[(=*)\[[\s\S]*?\]\2\]|"(?:\\[\s\S]|[^"\\])*"|\'(?:\\[\s\S]|[^\'\\])*\'')


def without_comments(text, language='cpp'):
    if language == 'xml':
        return re.sub(r'<!--[\s\S]*?-->', '', text)
    pattern = LUA if language == 'lua' else CPP
    prefixes = ('--',) if language == 'lua' else ('//', '/*')
    return pattern.sub(lambda m: re.sub(r'[^\r\n]', ' ', m[0])
                       if m[0].startswith(prefixes) else m[0], text)


def canonical(text, language='cpp'):
    text = without_comments(text, language)
    pattern = LUA if language == 'lua' else CPP
    result, offset = [], 0
    for match in pattern.finditer(text):
        result.append(re.sub(r'\s+', '', text[offset:match.start()]))
        result.append(match[0])
        offset = match.end()
    result.append(re.sub(r'\s+', '', text[offset:]))
    return ''.join(result)
