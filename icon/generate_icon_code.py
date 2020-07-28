line_len = 60


data_file = open('zm-icon.data', 'rb')
data = data_file.read()

row_count = 0
retVal = ""
for i in data:
    retVal += str(i) + ","
    
    row_count += 1
    if row_count > line_len:
        retVal += "\n"
        row_count = 0
        
print (retVal)