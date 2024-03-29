#include <iostream>
#include <bitset>
#include <math.h>
#include <fstream>

using namespace std;

// some specified places of
// the one from the registers that will feed back
const int fbA = 68;
const int fbB = 77;
const int fbC = 86;

// the feed forward one
const int ffA = 66;
const int ffB = 68;
const int ffC = 65;

void shiftMe(char *sr, const int s);
string getBinaryToText(string toConvert);
string getTextToBinary(string toConvert);
void initalizeSRs(char *sr, string initalizeWith, const int s);
int streamGenerator(char *srA, char *srB, char *srC, const int sA, const int sB, const int sC);

int main(int argc,char* argv[])
{
    // forcing to choose input mode
    if( !((argc >= 2) && (*argv[1] == 'f' || *argv[1] == 'c')) )
    {
        cout<<"\n\t\t     :::::::::: Please specify the input modes :::::::::: "<<endl;
        cout<<"\n\t\t    f for reading the data from file to encrypt or decrypt "<<endl;
        cout<<"\t      c for reading the data from terminal(input) to encrypt or decrypt"<<endl;
        cout<<"\ta for reading the data from file for IV and KEY; IV on first line and Key on second  \n"<<endl;

        cout<<"\n\t\t    order : object file f/c a/(nothing for terminal) \n"<<endl;
       

        return 0;
    }
 
    char option = '0';

    string IV; //iv from the user
    string key; //key from the user

    string plainText = "";
    string cipherText = "";

    string bitConvertedPlainText = "";  // the one that has converted to bits xi
    string bitConvertedCipherText = ""; // the one that has converted to bits yi
    int streamGenerated = 0;            // the si generated by the stream generator
    fstream file;

    // take IV and Key input

    // the constant sizes
    const int shiftRegisterA = 93;
    const int shiftRegisterB = 84;
    const int shiftRegisterC = 111;

    // the three registers
    char LFSRA[shiftRegisterA];
    char LFSRB[shiftRegisterB];
    char LFSRC[shiftRegisterC];

again:
    plainText = "";
    cipherText = "";

    bitConvertedPlainText = "";
    bitConvertedCipherText = "";
    streamGenerated = 0;

    cout << "\n\t ::::::::::::::::::::::::::::::::" << endl;
    cout << "\t :::::::: Trivium Cipher ::::::::" << endl;
    cout << "\t ::::::::::::::::::::::::::::::::\n"
         << endl;

    cout << "\n 1 for encryption \n 2 for decryption \n other for exit " << endl;

    cout << "\n Enter the desired Option : ";
    cin >> option;

    cin.ignore();

    int a  = 0;

    if( option == '1' || option == '2')
    {

         // taking the key and iv values 80 bit strictly
        do
        {
            
            if(a>0 && (argc == 3 && *argv[2] == 'a'))
            {
                 cout<<"\n\t\t\t  ::::: Bad Input :::::"<<endl;
                cout<<"\n\t\t : Only 10 characters - update file please :\n"<<endl;
                return 0; //update the file
            }

            if(a > 0)
                cout<<"\n ::: IV and key must be 80 bit .... Kindly enter 10 characters only :::"<<endl;

            // making outmated with files
            if( argc == 3  && *argv[2] == 'a')
            {
                // read file
                if(option == '1') 
                    file.open("EnIvkey.txt" , ios::in);
                else
                    file.open("DecIvkey.txt" , ios::in);


                if(file)
                {
                    // reading only two
                    getline(file, IV);
                    getline(file, key);

                    file.close();
                }
                else
                {
                    cout<<"\n\t\t\t    :::::::::: For Iv and Key file kindly make files  :::::::::: \n:::::::::: 'EnIvkey.txt' that will use in encryption and  'DecIvkey.txt' that will use in decryption :::::::::: \n"<<endl;
                    return 0;
                }
             
            }
            else
            {
                cout<<"\n Enter the 80-bit IV  : ";
                getline(cin, IV);

                cout<<" Enter the 80-bit key : ";
                getline(cin, key);

            }

            a++;

        } while(!((IV.length() == 10) && (key.length() == 10))); // if less than 10 or more than 10 then ask again
        
        // converting iv and key to binary
        IV = getTextToBinary(IV);
        key = getTextToBinary(key);

        // initalizing
        initalizeSRs(LFSRA, IV, shiftRegisterA);
        initalizeSRs(LFSRB, key, shiftRegisterB);
        initalizeSRs(LFSRC, "", shiftRegisterC);
        LFSRC[shiftRegisterC - 3] = LFSRC[shiftRegisterC - 2] = LFSRC[shiftRegisterC - 1] = '1';

    }

    switch (option)
    {
    case '1':
        // -------------------------------------
        //               encryption
        // -------------------------------------

        cout << "\n\t :::::::::::::::: Encrypting ::::::::::::::::::" << endl;
        
         if(*argv[1] == 'c')
        {
            cout << "\n\n Enter the message to encrypt : ";
            getline(cin, plainText);
        }
        else
        {
            string fname;

            while(true)
            {
                fname = "";
                cout << "\n\n Enter the file name : ";
                getline(cin, fname);

                file.open(fname , ios::in); // opening in reading mode

                // if opened 
                if(file)
                {
                    string message;

                    // reading the whole file
                    while(!file.eof())
                    {
                        getline(file, message);

                        // if ending line then not add \n
                        if(file.eof())
                            plainText+=message;
                        else
                            plainText+=message + "\n";
                    }

                    file.close();
                    break; // ok
                }
                else
                {
                    cout<<"\n\t ::::: Bad Input :::::"<<endl;
                    continue; // ask again
                }
                
            }

           
        }

        // converting plain to binary
        bitConvertedPlainText = getTextToBinary(plainText); // converted the xi
        bitConvertedCipherText = bitConvertedPlainText;     // making size compatiable .. all values will be changed in the loop

        // warming up so to make complete random
        for(int i = 0 ; i < 1152 ; i++)
            streamGenerator(LFSRA, LFSRB, LFSRC, shiftRegisterA, shiftRegisterB, shiftRegisterC);

        // now getting the each bit of plain and encrypting with the generator
        for (int i = 0; i < bitConvertedPlainText.length(); i++)
        {
            // the si that will be used for the encryption
            streamGenerated = streamGenerator(LFSRA, LFSRB, LFSRC, shiftRegisterA, shiftRegisterB, shiftRegisterC);
            bitConvertedCipherText[i] = (((bitConvertedCipherText[i] - 48) ^ streamGenerated) + 48); // the cipher bit made
        }

        // storing the encrypted text
        cipherText = getBinaryToText(bitConvertedCipherText);

        cout << " Encrypted message is : " << cipherText << endl
             << endl;
        
        // writing to file
        file.open("encrypted.txt" , ios::out); // opening in reading mode
        file << cipherText;
        file.close();

        cout<<"\n ::::: Written to encrypted.txt also :::::::: \n"<<endl;
    
        goto again;
    case '2':
        // -------------------------------------
        //               decryption
        // -------------------------------------

        cout<< "\n\t :::::::::::::::: Decrypting ::::::::::::::::::"<<endl;

        if(*argv[1] == 'c')
        {
            cout << "\n\n Enter the message to decrypt : ";
            getline(cin, cipherText);
        }
        else
        {
            string fname;
            while(true)
            {
                fname = "";
                cout << "\n\n Enter the file name : ";
                getline(cin, fname);

                file.open(fname , ios::in); // opening in reading mode

                // if opened 
                if(file)
                {
                    string message;
                    
                    // reading the whole file
                    while(!file.eof())
                    {
                        getline(file, message);
                        cipherText+=message;
                    }

                    file.close();
                    break; // ok
                }
                else
                {
                    cout<<"\n\t ::::: Bad Input :::::"<<endl;
                    continue; // ask again
                }
            }
        }

        // converting cipher to binary
        bitConvertedCipherText = getTextToBinary(cipherText); // converted the xi
        bitConvertedPlainText = bitConvertedCipherText;      // making size compatiable .. all values will be changed in the loop

         // warming up so to make complete random
        for(int i = 0 ; i < 1152 ; i++)
            streamGenerator(LFSRA, LFSRB, LFSRC, shiftRegisterA, shiftRegisterB, shiftRegisterC);

        // now getting the each bit of cipher and decrypting with the generator
        for (int i = 0; i < bitConvertedCipherText.length(); i++)
        {
            streamGenerated = streamGenerator(LFSRA, LFSRB, LFSRC, shiftRegisterA, shiftRegisterB, shiftRegisterC);
            bitConvertedPlainText[i] = (((bitConvertedCipherText[i] - 48) ^ streamGenerated) + 48);
        }

        // storing the encrypted text
        plainText = getBinaryToText(bitConvertedPlainText);

        cout << " Real message is : " << plainText << endl
             << endl;

        // writing to file
        file.open("decrypted.txt" , ios::out); // opening in reading mode
        file << plainText;
        file.close();
    
        cout<<"\n ::::: Written to decrypted.txt also :::::::: \n"<<endl;

        goto again;

    default:
        cout << "\n\t :::::::::::::::: Exiting ::::::::::::::::::" << endl
             << endl;
    }

    return 0;
}

// will initalize accordingly
void initalizeSRs(char *sr, string initalizeWith, const int s)
{
    // initalizing with given
    for (int i = 0; i < initalizeWith.length(); i++)
        sr[i] = initalizeWith[i];

    // for remainig zero
    for (int i = initalizeWith.length(); i < s; i++)
        sr[i] = '0';

    return;
}

// for shifting bits one to right after output
void shiftMe(char *sr, const int s)
{
    // shifiting each bit to right except first one and array will be changed as passed by value
    for (int i = s - 1; i > 0; i--)
        sr[i] = sr[i - 1]; // assign previous one to me

    return;
}

// will generates the stream s by xoring and the whole trivium algorithm
// and return the strem such as si and so on till needed
int streamGenerator(char *srA, char *srB, char *srC, const int sA, const int sB, const int sC)
{
    // storing the individual results
    int shiftA, shiftB, shiftC, result;
    shiftA = shiftB = shiftC = result = 0;

    // for the output first of all the out put from shift a
    shiftA = (srA[sA - 1] - 48) ^ (srA[ffA] - 48) ^ ((srA[sA - 2] - 48) & (srA[sA - 3] - 48));
    shiftB = (srB[sB - 1] - 48) ^ (srB[ffB] - 48) ^ ((srB[sB - 2] - 48) & (srB[sB - 3] - 48));
    shiftC = (srC[sC - 1] - 48) ^ (srC[ffC] - 48) ^ ((srC[sC - 2] - 48) & (srC[sC - 3] - 48));

    // now we have the three results thus finding the desired result
    result = shiftA ^ shiftB ^ shiftC;

    // doing other work such as updatng arrays etc
    // deciding the one that we need to feed back calculating that
    int feedA, feedB, feedC;
    feedA = feedB = feedC = 0;

    // deciding the feed value
    feedA = shiftC ^ (srA[fbA] - 48);
    feedB = shiftA ^ (srB[fbB] - 48);
    feedC = shiftB ^ (srB[fbB] - 48);

    // thus now shifting and making space for the feed back
    shiftMe(srA, sA);
    shiftMe(srB, sB);
    shiftMe(srC, sC);

    // thus inserting the feed backs and returning the si for the encryption
    // or decryption of the plain text or cipher text respectively.
    srA[0] = feedA + 48;
    srB[0] = feedB + 48;
    srC[0] = feedC + 48;

    return result; // the si
}

// will convert the given string accordingly
string getBinaryToText(string toConvert)
{
    string result = "";
    int ascii = 0;

    // as we deal with the 8 bits thus
    for (int i = 0; i < toConvert.length(); i += 8)
    {
        ascii = 0;
        
        for (int j = i, count = 7; j< 8+i  ; j++)
        {
            // converting
            ascii += (toConvert[j] - 48) * pow(2, count );
            count--;
        }

        result+= ascii;
    }

    return result; // returning the result
}

//  will convert text to binary containing text
string getTextToBinary(string toConvert)
{
    string result = "";

    // as each charter is is 8 bit thus making each a eight bit
    // and appending to the resultant string
    for (int i = 0; i < toConvert.length(); i++)
        result += bitset<8>(toConvert[i]).to_string();

    // returing the converted
    return result;
}
