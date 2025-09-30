import hashlib

def sanitizer(file):
    file = file.replace('\n', '').replace('\r', '').replace('\t', '')
    bfile = file.encode('utf-8')
    return bfile

def cmpHash(text1, text2):
    hashA = hashlib.md5()
    hashA.update(text1)
    hashA = hashA.hexdigest()

    hashB = hashlib.md5()
    hashB.update(text2)
    hashB = hashB.hexdigest()

    return hashA == hashB

def cmpBytebyByte(text1, text2):
    return text1 == text2

def main(prog_file):
    file = open("active_program", "r")
    cntfile = file.read().replace('\r','').replace('\n','')
    path = "./st_files/"
    try:
        contentA = open(path+cntfile, 'r')
        contentB = open(path+prog_file, 'r')
    except:
        print("There is no file for comparision. ")
        return 404, 404
    
    dataA = contentA.read()
    dataB = contentB.read()

    dataA = sanitizer(dataA)
    dataB = sanitizer(dataB)
    
    if len(dataA) != len(dataB):
        hash_result = cmpHash(dataA, dataB)
        byte_compare = cmpBytebyByte(dataA, dataB)
        return hash_result, byte_compare
    else:
        hash_result = cmpHash(dataA, dataB)
        byte_compare = cmpBytebyByte(dataA, dataB)
        return hash_result, byte_compare

if __name__ == '__main__':
    print()
