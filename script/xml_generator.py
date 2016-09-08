from pyparsing import *

expr = Forward()

double = Word(nums + ".").setParseAction(lambda t:float(t[0]))
integer = Word(nums).setParseAction(lambda t:int(t[0]))
variable = Word(alphas)
funccall = Group(variable + nestedExpr('(', ')', delimitedList(expr)))
operand = double | integer | funccall | variable

expop = Literal('^')
signop = oneOf('+ -')
multop = oneOf('* /')
plusop = oneOf('+ -')
factop = Literal('!')

expr << operatorPrecedence( operand,
    [("!", 1, opAssoc.LEFT),
     ("^", 2, opAssoc.RIGHT),
     (signop, 1, opAssoc.RIGHT),
     (multop, 2, opAssoc.LEFT),
     (plusop, 2, opAssoc.LEFT),]
    )
import xml.etree.cElementTree as ET

def flatten_unit(results):
    flat_result = results
    if (isinstance(results, list) or isinstance(results, ParseResults)) and len(results) == 1:
        flat_result = results[0]
    return flat_result


####################################
# Single tokens
###################################

def is_single_token(expression):
    """
    Want to catch 3 'y' [3] ['z']
    """
    expression = flatten_unit(expression)
    if isinstance(expression, float) or isinstance(expression, int):
        return ('constant', str(expression))
    if isinstance(expression, str):
        return ('variable', expression)
    
    return None

def add_single_token(tree, single_token, to_single_token):
    token_type, value = (single_token)
    if to_single_token: value = to_single_token(value)
    ET.SubElement(tree, token_type, value=str(value))

####################################
# Unary
###################################

UNARY_TOKENS = ['!', '-', 'exp', 'log', 'sqrt']

def is_unary(results):
    """
    - ! exp(), etc...
    """
    return len([token for token in results if token in UNARY_TOKENS]) > 0

def add_unary_expression(tree, results):
    # we treat the factorial specially
    if "!" in results.asList():
        new_node = ET.SubElement(tree, "un_op", type='!')
        add_expression(new_node, results[0], lambda x: "%.0f" % float(x))
        # Hack to have only int
    else:
        new_node = ET.SubElement(tree, "un_op", type=results[0])
        add_expression(new_node, flatten_unit(results[1]))

def add_binary_expression(tree, op_type, name, left, right):
    new_node = ET.SubElement(tree, op_type, type=name)
    add_expression(new_node, left)
    add_expression(new_node, right)

def add_expression(tree, results, to_single_token = None):
    """
    We need to add the right stuff
    """
    # Single tokens
    single_token = is_single_token(results)
    if single_token:
        add_single_token(tree, single_token, to_single_token)
        return
    
    # Two tokens
    if len(results) == 2:
        is_unary_expression = is_unary(results)
        if is_unary_expression:
            add_unary_expression(tree, results)
        else:
            # it's a function of two arguments
            add_binary_expression(tree, "bin_op", results[0], results[1][0], results[1][1])

    if len(results) >= 3:
        # At least 3 tokens
        right = results[2] if len(results) == 3 else results[2:]
        add_binary_expression(tree, "bin_op", results[1], results[0], right)

def process_line(full_line):
    return [s.strip() for s in full_line.split('=')]

def create_xml(lines, fname, display = True):
    root = ET.Element("root")
    tree = ET.ElementTree(root)
    
    for line in lines:
        (expression_name, expression) = process_line(line)
        root.append(ET.Comment(line))
        parsed_expression = expr.parseString(expression)[0]
        expression_node = ET.SubElement(root, "variable", value=expression_name)
        add_expression(expression_node, parsed_expression)

    xml_string = tree.write(fname)

    if display:
        xml_string = ET.tostring(root, encoding = 'utf-8')
        pretty_print(xml_string)
    

def pretty_print(xml_string):
    from lxml import etree
    root = etree.fromstring(xml_string)
    print (str(etree.tostring(root, pretty_print = True), 'utf-8'))

tests = ["X = 3",
        "Y = z",
        "Z = 3!",
        "X = -3",
        "X = log(z)",
        "X = min(1, 3)",
        "X = max(2, 3!)",
        "Y = x + 3",
        "x = 9 + 2 + 3.5",
        "Z = log(1+z)",
        "Z = log(1+3)",
        "x = 9 + (2 * 3)",
        "x = 9 + 2 * 3",
        "x = (9 + -2) * 3",
        "x = (9 + -var) * 3",
        "x = (9 + -2) * 3^2^2",
        "x = Matrix*X + B",
        "x = M*(X + B)",
        "E = log(1+y) + 3*min(2,3)",]
#create_xml(tests[-1:], 'sample.xml')
