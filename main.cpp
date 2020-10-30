#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <iomanip>

using namespace std;

struct Matrix{
    float matrix[100][101];
    int numberOfEquations;
};

Matrix loadMatrix(string fileName)
{ // Loads matrix from file to structure and returns it
    ifstream file;
    file.open(fileName);

    Matrix mat;
    file >> mat.numberOfEquations;

    for(int row = 0; row < mat.numberOfEquations; row++)
    {
        for(int column = 0; column < mat.numberOfEquations + 1; column++)
        {
            file >> mat.matrix[row][column];
        }
    }
    file.close();

    return mat;
}

void linesPrint(int length = 47, string text = "", bool special = false)
{ // Prints lines to command line with optional text to make better UX experience
    int i, txtSize;

    if(text == "")
    {
        for(i = 0; i < length; i++)
        {
            cout << "-";
        }
        cout << "\n\n";
    }
    else
    {
        txtSize = text.size();
        if(special)
        {
            cout << "    ";
            for(i = 0; i < txtSize + 2; i++)
            {
                cout << "-";
            }
            cout << "\n";
        }

        cout << "---- " << text << " ";
        for(i = 0; i < length - 6 - txtSize; i++)
        {
            cout << "-";
        }

        if(special)
        {
            cout << "\n    ";
            for(i = 0; i < txtSize + 2; i++)
            {
                cout << "-";
            }
        }
        cout << "\n\n";
    }
}

void printMatrix(Matrix mat)
{ // Prints matrix to command line
    for(int i = 0; i < mat.numberOfEquations; i++){
        for(int j = 0; j < mat.numberOfEquations + 1; j++){
            if(j == mat.numberOfEquations)
            {
                cout << "| ";
            }
            cout << std::fixed;
            cout << setprecision(3);
            cout << setw(8) << mat.matrix[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

Matrix interchangeRows(Matrix mat, int row1, int row2)
{ // Interchanges 2 rows in a matrix
    Matrix temp;
    for(int i = 0; i < mat.numberOfEquations + 1; i++)
    {
        temp.matrix[0][i] = mat.matrix[row1][i];
    }
    for(int j = 0; j < mat.numberOfEquations + 1; j++)
    {
        mat.matrix[row1][j] = mat.matrix[row2][j];
    }
    for(int k = 0; k < mat.numberOfEquations + 1; k++)
    {
        mat.matrix[row2][k] = temp.matrix[0][k];
    }

    return mat;
}

Matrix checkForDiagonalZeros(Matrix mat, char *errorReturn)
{ // Checks matrix for zeros in main diagonal and if it finds some, it tries to interchange rows so that no zeros will be in main diagonal
    int row, k, tempK;
    int recursionCounter = 0; // variable to prevent recursion
    bool zeroFound = false; // Checks whether at least once was matrix modified because of zero found on main diagonal
    bool cleanRun = true; // Checks whether one run of checking zeros on man diagonal was clean (without zeros)
    bool differentMatrices = false; // Checks whether initial and modified matrices are different or not
    Matrix check;

    do
    {
        cleanRun = true;
        for(row = 0; row < mat.numberOfEquations; row++){
            if(mat.matrix[row][row] == 0) // Zero in main diagonal found
            {
                cleanRun = false;
                if(!zeroFound) // make initial matrix copy for later control
                {
                    zeroFound = true;

                    for(k = 0; k < mat.numberOfEquations; k++)
                    {
                        for(tempK = 0; tempK < mat.numberOfEquations + 1; tempK++)
                        {
                            check.matrix[k][tempK] = mat.matrix[k][tempK];
                        }
                    }
                }

                for(k = 1; k < mat.numberOfEquations; k++) // searching new good row
                {
                    if((row + k) >= mat.numberOfEquations) // search for new good row reached end of matrix, continuing from top of matrix
                    {
                        tempK = k - mat.numberOfEquations;
                    }
                    else
                    {
                        tempK = k;
                    }

                    if(mat.matrix[row + tempK][row] != 0) // good row with number other than zero found
                    {
                        mat = interchangeRows(mat, row + tempK, row);
                        break;
                    }

                    if(k+1 == mat.numberOfEquations) // column with only zeros found
                    {
                        *errorReturn = 1;
                        return mat;
                    }
                }
            }
        }

        if(zeroFound) // Check whether initial matrix copy and modified matrix are different matrices
        {
            for(row = 0; row < mat.numberOfEquations; row++)
            {
                for(int k = 0; k < mat.numberOfEquations + 1; k++)
                {
                    if(check.matrix[row][k] != mat.matrix[row][k])
                    {
                        differentMatrices = true;
                        break;
                    }
                }
            }
            if(differentMatrices)
            {
                differentMatrices = false;

                recursionCounter++;
                if(recursionCounter > mat.numberOfEquations) // check whether modified matrix repeats by setting it as check matrix
                {
                    zeroFound = false;
                }
            }
            else // Matrices are the same -> error
            {
                *errorReturn = 2;
                return mat;
            }
        }
    }while(!cleanRun);

    return mat;
}

Matrix multiplyByNumber(Matrix mat, float number, int row)
{ // Multiplies numbers in row by number
    for(int i = 0; i < mat.numberOfEquations + 1; i++){
        mat.matrix[row][i] *= number;
    }

    return mat;
}

Matrix addingRows(Matrix mat, int from, int into)
{ // Adds numbers from one row into the other row
    for(int i = 0; i < mat.numberOfEquations + 1; i++)
    {
        mat.matrix[into][i] += mat.matrix[from][i];
    }
    return mat;
}

bool intChecker(float a)
{ // Checks whether number is integer or not
    if(fmod(a, 1) != 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

float gcdSolver(float a, float b)
{ // Finds greatest common divisor (gcd) between two integer numbers
    while(true)
    {
        if(a > b)
        {
            a = fmod(a, b);
        }
        else
        {
            b = fmod(b, a);
        }

        if(a == 0 || b == 0)
        {
            break;
        }
    }

    if(a == 0)
    {
        return b;
    }
    else
    {
        return a;
    }
}

Matrix gcdRowCheck(Matrix mat, int row, int fromColumn, char *errorReturn)
{ // Checks whether it's possible to divide row with integer number to preserve small numbers in matrix and prevent variable overflow
    float a, b;
    a = fabs(mat.matrix[row][fromColumn]);

    if(!intChecker(a))
    {
        *errorReturn = 3;
        return mat;
    }

    if(a == 0)
    {
        return mat;
    }

    for(int i = fromColumn + 1; i < mat.numberOfEquations + 1; i++)
    {
        b = fabs(mat.matrix[row][i]);

        if(!intChecker(b))
        {
            *errorReturn = 3;
            return mat;
        }
        if(b == 0)
        {
            continue;
        }

        a = gcdSolver(a, b);

        if(a == 1)
        {
            return mat;
        }
    }
    mat = multiplyByNumber(mat, (1/a), row);

    return mat;
}

Matrix sameRows(Matrix mat, int diagonalRow, int secondInColumn, char *errorReturn)
{ // Finds smallest possible numbers to multiply rows so that after adding one row to the other, zero appears
    float a = fabs(mat.matrix[diagonalRow][diagonalRow]);
    float b = fabs(mat.matrix[secondInColumn][diagonalRow]);

    if(b == 0) // Number is already zero, no need to do anything
    {
        return mat;
    }

    if(!intChecker(a) || !intChecker(b)) // Number isn't an integer -> error
    {
        *errorReturn = 3;
        return mat;
    }

    float gcd = gcdSolver(a, b);

    a /= gcd; // Calculating numbers by which we should multiply rows so that we can later add rows together and get zero
    b /= gcd;

    if(mat.matrix[diagonalRow][diagonalRow] < 0)
    {
        b *= -1;
    }
    if(mat.matrix[secondInColumn][diagonalRow] > 0)
    {
        a *= -1;
    }

    mat = multiplyByNumber(mat, b, diagonalRow);
    mat = multiplyByNumber(mat, a, secondInColumn);

    mat = addingRows(mat, diagonalRow, secondInColumn);

    mat = multiplyByNumber(mat, 1/b, diagonalRow); // Dividing first row back to smaller numbers

    return mat;
}

Matrix cleaning(Matrix mat, int row)
{ // Final adding and multiplying numbers together to get identity matrix
    float variable = (mat.matrix[row][mat.numberOfEquations]) / (mat.matrix[row][row]);
    int i, num;

    mat.matrix[row][mat.numberOfEquations] /= mat.matrix[row][row];
    mat.matrix[row][row] /= mat.matrix[row][row];

    if(row > 0){
        for(i = row - 1; i > -1; i--)
        {
            num = mat.matrix[i][row] * variable;
            mat.matrix[i][row] = 0;
            mat.matrix[i][mat.numberOfEquations] -= num;
        }
    }

    return mat;
}


int main()
{
    Matrix a;

    char errorReturn = 0;
    char answer;
    bool detailed = false;

    a = loadMatrix("example.txt");

    linesPrint(47, "Loaded matrix", true);
    printMatrix(a);
    linesPrint();

    cout << "Would you like to see the individual\nsteps of solving this matrix?\n\ny/n ";
    cin >> answer;
    cout << "\n";
    if(answer == 'y' || answer == 'Y')
    {
        detailed = true;
    }

    if(detailed)
    {
        linesPrint(47, "Working matrix", true);
    }

    a = checkForDiagonalZeros(a, &errorReturn);

    if(detailed)
    {
        printMatrix(a);
        linesPrint(47, "Steps");
    }

    if(errorReturn != 0)
    {
        linesPrint(20, "Error");
        if(errorReturn == 1)
        {
            cout << "\nColumn with only zeros found!\nPlease delete this column along with one row of your choice (so that the number of unknowns and rows equals) and try again\n\n";
            return 0;
        }
        else if(errorReturn == 2)
        {
            cout << "\nError occurred while trying to interchange rows to avoid zeros on the main diagonal!\nThis program couldn't rearrange matrix's rows so that there would be no zeros on the main diagonal.\nPlease check whether you can edit this matrix into better order with less zeros and try again\n\n";
            return 0;
        }
    }

    int i, j;

    for(i = 0; i < a.numberOfEquations; i++)
    {
        a = gcdRowCheck(a, i, i, &errorReturn);

        for(j = i + 1; j < a.numberOfEquations; j++)
        {
            a = sameRows(a, i, j, &errorReturn);
            a = gcdRowCheck(a, j, i + 1, &errorReturn);

            if(errorReturn == 3)
            {
                linesPrint(20, "Error");
                cout << "\nNon-integer number found in an important part of matrix!\nPlease, make sure there are only integers in the matrix and try again\n\n";
                return 0;
            }
            if(detailed)
            {
                printMatrix(a);
            }
        }
    }

    if(detailed)
    {
        linesPrint();
    }

    for(i = a.numberOfEquations - 1; i > -1; i--)
    {
        a = cleaning(a, i);

        if(detailed)
        {
            printMatrix(a);
        }
    }

    linesPrint(47, "Final matrix", true);
    printMatrix(a);
    linesPrint();

    return 0;
}
