import os

KEYSIZE = 16
BLOCKSIZE = 16

def createKey(keysize):
    key = os.urandom(keysize)
    return key

def createIV(blocksize):
    iv = os.urandom(blocksize)
    return iv

def ivcheck():
    ivexists = os.path.exists('./iv.bin')
    return ivexists

def keycheck():
    keyexists = os.path.exists('./key.bin')
    return keyexists

def main():
    check1 = keycheck()
    check2 = ivcheck()
    if check1 == False or check2 == False:
        key = createKey(KEYSIZE)
        iv = createIV(BLOCKSIZE)

        with open("key.bin", 'wb') as keyfile:
            keyfile.write(key)
            keyfile.close()
            
        with open('iv.bin', 'wb') as ivfile:
            ivfile.write(iv)
            ivfile.close()
    else:
        pass
    
if __name__ == '__main__': 
    main()