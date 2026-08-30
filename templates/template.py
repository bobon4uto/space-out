import sys

if (len(sys.argv) < 4):
    print("usage: template template_file variables_file output_file")
    exit(1)



class Variables:
    def __init__(self, text):
        self.dictianory = {}
        for line in text.splitlines():
            equasion = line.split("=")
            self.dictianory[ equasion[0] ] = equasion[1]

def apply_variables_to_template(template:str, variables:str):
    var_table = Variables(variables)
    parts = template.split("`")
    result = ""


    for part in parts:
        if part in var_table.dictianory.keys():
            result += var_table.dictianory[part]
        else:
            result += part
    return result



with open(sys.argv[1], "r") as template_file:
    with open(sys.argv[2], "r") as variables_file:
        template  = template_file.read()
        variables = variables_file.read()
        output = apply_variables_to_template(template, variables)
        with open(sys.argv[3], "w") as output_file:
            output_file.write(output)
        

