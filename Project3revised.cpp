//Created by Mark Mondt mjm170130

#include <iostream>
#include <iomanip>
#include <cmath>
#include <cctype>
#include <fstream>

using namespace std;

struct node
{
   int row;
   char seat;
   bool reserved;
   char type;
   node* Up = nullptr;
   node* Down = nullptr;
   node* Right = nullptr;
   node* Left = nullptr;
};

void Display(fstream&, node*, int);

int RowLength(fstream&);

int AmountRows(fstream&, int);

void CreateList(fstream&, node*, int);

void Input(fstream&, node*, int);

void DeleteList(node*&);

void SeatInput(fstream&, int&, char&, int&, int&, int&, int);

bool Available(node*, int, char, int, int);

node* BestSeats(fstream&, node*, int, int, int ,int);

void ReserveSeats(fstream&, node*, int, char, int, int, int, int);

int Tabulate(fstream&, node*, int, char);

int main()
{
   //declare variables
   int MenuIn = 1;
   char ReserveIn = 'N';
   int LengthRow;
   const double AdultPrice = 10.00;
   const double ChildPrice = 5.00;
   const double SeniorPrice = 7.50;
   int RowChoice, AdultNum, ChildNum, SeniorNum;
   char SeatChoice;
   node* head = new node;
   node* BestSeat;

   //create file stream object and connect to file
   fstream Audit;
   Audit.open("A1.txt", ios:: out | ios:: in | ios:: binary);
   if (Audit.good())
      {
         LengthRow = RowLength(Audit);
         CreateList(Audit, head, LengthRow);
         Input(Audit, head, LengthRow);

         do
         {
            //display message if previous input was invalid
            if (MenuIn != 1 && MenuIn != 2)
            {
               cout << "Please enter 1 or 2 for menu choice" << endl;
               cin.clear();
               cin.ignore(23000, '\n');
            }

            //prompt and receive menu choice
            cout << "1. Reserve Seats" << endl;
            cout << "2. Exit" << endl;
            cin >> MenuIn;


            if (MenuIn == 1)
            {
               //call function to display auditorium
               Display(Audit, head, LengthRow);

               //call function to receive input
               SeatInput(Audit, RowChoice, SeatChoice, AdultNum, ChildNum, SeniorNum, LengthRow);

               //call function to check seat availability
               if (Available(head, RowChoice, SeatChoice, AdultNum + ChildNum + SeniorNum, LengthRow))
               {
                  cout << "Your selected seats were available" << endl;

                  //If preferred seats are available, reserve them
                  ReserveSeats(Audit, head, RowChoice, SeatChoice, AdultNum, ChildNum, SeniorNum, LengthRow);
               }
               else
               {
                  cout << "Your selected seats were not available" << endl;

                  //If preferred seats are unavailable, prompt with an alternate solution if it exists
                  BestSeat = BestSeats(Audit, head, AdultNum, ChildNum, SeniorNum, LengthRow);

                  if (BestSeat == nullptr)
                     cout << "There were no alternative seats" << endl;
                  else
                     {
                     //display the best available found
                     cout << "Best available seats found at row " << BestSeat->row << ", seats "
                        << BestSeat->seat << "-" << static_cast<char>(BestSeat->seat + AdultNum + ChildNum + SeniorNum - 1) << endl;

                     //prompt user to accept or decline best available
                     cout << "Reserve these seats? (Y/N)" << endl;
                     cin >> ReserveIn;

                     if (toupper(ReserveIn) == 'Y')
                        ReserveSeats(Audit, head, BestSeat->row, BestSeat->seat, AdultNum, ChildNum, SeniorNum, LengthRow);
                     }
               }
            }
         }while (MenuIn != 2);

         //display output
         cout << left << endl;
         cout << setw(30) << "Total Seats in Auditorium: " << LengthRow * AmountRows(Audit, LengthRow) << endl;
         cout << setw(30) << "Total Tickets Sold: " << Tabulate(Audit, head, LengthRow, 'A') + Tabulate(Audit, head, LengthRow, 'S') + Tabulate(Audit, head, LengthRow, 'C') << endl;
         cout << setw(30) << "Adult Tickets Sold: " << Tabulate(Audit, head, LengthRow, 'A') << endl;
         cout << setw(30) << "Child Tickets Sold: " << Tabulate(Audit, head, LengthRow, 'C') << endl;
         cout << setw(30) << "Senior Tickets Sold: " << Tabulate(Audit, head, LengthRow, 'S') << endl;
         cout << fixed << showpoint << setprecision(2);
         cout << setw(30) << "Total Ticket Sales: " << "$" <<
         AdultPrice * Tabulate(Audit, head, LengthRow, 'A') + SeniorPrice * Tabulate(Audit, head, LengthRow, 'S') + ChildPrice * Tabulate(Audit, head, LengthRow, 'C') << endl;


         Audit.close();

         DeleteList(head);
      }
   else
      //display error message because the file did not open properly
      cout << "Error: File did not open properly" << endl;


   return 0;
}



//This function displays the available and taken seats within the auditorium
void Display(fstream& File, node* head, int Length)
{
   int rows = AmountRows(File, Length);
   node* rowhead = head;

   cout << endl;
   cout << endl << "  ";

   for (char letter = 'A'; letter < 'A' + Length; letter++)
      cout << letter;
   cout << endl;

   //step through each element of the grid
   for (int r = 1; r <= rows; r++)
   {
      cout << r << " ";
      for (char i = 'A'; i < 'A' + Length; i++)
      {
        if (toupper(head->type) == 'A' || toupper(head->type) == 'S' || toupper(head->type) == 'C')
         cout << "#";
        else
         cout << head->type;

        head=head->Right;
      }

      cout << endl;

      rowhead = rowhead->Down;
      head = rowhead;
   }
   cout << endl;
}



//This function determines the length of each row including the newline
int RowLength(fstream& Audit2)
{
   char ReadIn = ' ';
   bool Finished = false;
   int Length = -1;

   //position file pointer at the beginning of the file
   Audit2.seekg(0L, ios::beg);

   //find first newline character and return position
   while (!Finished)
   {
      Audit2.get(ReadIn);

      if (Audit2.eof())
         Finished = true;

      if (ReadIn == '\n' || ReadIn == '\r')
         Finished = true;

      Length++;
   }

   Audit2.clear();

   return Length;
}



//This function determines the amount of rows in the file
int AmountRows (fstream& Audit, int Length)
{
   long long int pos;

  //position file pointer at the end of the file
  Audit.seekg(0L, ios::end);

  Audit.clear();
  pos = Audit.tellg();

  //determine how many rows fit in the file
  return (pos + 2) / (Length + 1);
}



//creates linked grid to match dimensions
void CreateList(fstream& File, node* ptr, int rowlength)
{
   int rows = AmountRows(File, rowlength);
   node* Up = ptr;
   node* rowhead = ptr;

   //create and connect the nodes of the auditorium
   for (int i = 1; i < rowlength; i++)
   {
      ptr->Right = new node;
      ptr->Right->Left = ptr;
      ptr = ptr->Right;
   }

   rowhead->Down = new node;
   ptr = rowhead->Down;
   Up = rowhead;
   rowhead=ptr;
   rowhead->Up = Up;
   Up->Down = rowhead;

   for (int r = 1; r < rows; r++)
   {
      for (int i = 1; i < rowlength; i++)
      {
         ptr->Right = new node;
         ptr->Right->Left = ptr;
         ptr = ptr->Right;
         Up = Up->Right;
         Up->Down = ptr;
         ptr->Up = Up;
      }

      if (r < rows - 1)
      {
         rowhead->Down = new node;
         ptr = rowhead->Down;
         Up = rowhead;
         rowhead = ptr;
         ptr->Up = Up;
      }
   }
}




//receive values from the file and put them in the linked grid
void Input(fstream& file, node* head, int rowlength)
{
   int rows = AmountRows(file, rowlength);
   char ReadIn;
   node* rowhead = head;

   file.seekg(0L, ios::beg);

   //enter values into the linked grid
   for (int r = 1; r <= rows; r++)
   {
      for (char i = 'A'; i < 'A' + rowlength; i++)
      {
        file.get(ReadIn);

        if(ReadIn == '\n' || ReadIn == '\r')
        {
           i--;
           continue;
        }

        head->type = ReadIn;
        head->seat = i;
        head->row = r;
        if (ReadIn == '.')
         head->reserved = false;
        else
         head->reserved = true;

         head = head->Right;
      }

      rowhead = rowhead->Down;
      head = rowhead;

   }
}




//dismantles and deletes nodes of the linked grid
void DeleteList(node*& ptr)
{
   node* rowhead = ptr;

   //disconnect and delete nodes

   while (rowhead != nullptr)
   {
      while (ptr->Right != nullptr)
      {
         ptr = ptr->Right;
         ptr->Left->Up = nullptr;
         ptr->Left->Left = nullptr;
         ptr->Left->Right = nullptr;
         ptr->Left->Down = nullptr;
         delete ptr->Left;
      }

      ptr->Up = nullptr;
      ptr->Left = nullptr;
      ptr->Right = nullptr;
      ptr->Down = nullptr;
      delete ptr;

      rowhead = rowhead->Down;
      ptr = rowhead;
   }

   ptr = nullptr;
}




//receives and validates input
void SeatInput(fstream& Audit, int& Row, char& Seat, int& Adult, int& Child, int& Senior, int Length)
{
   Row = Adult = Child = Senior = 1;
   Seat = 'A';
   int Rows = AmountRows(Audit, Length);

   //receive and validate Row Number
   do
   {
      if (Row < 1 || Row > Rows)
      {
         cout << "Row Number must match diagram above" << endl;
         cin.clear();
         cin.ignore(23000, '\n');
      }

      cout << "What row do you want?" << endl;
      cin >> Row;
   }while (Row < 1 || Row > Rows);

   //receive and validate Seat Letter
   do
   {
      if (Seat < 'A' || Seat >= 'A'+ Length)
      {
         cout << "Seat letter must match diagram above" << endl;
         cin.clear();
         cin.ignore(23000, '\n');
      }

      cout << "What letter do you want your seats to start on (Left to Right)" << endl;
      cin >> Seat;
      Seat = toupper(Seat);
   }while (Seat < 'A' || Seat >= 'A'+ Length);

   //receive and validate Adult ticket number
   do
   {
      if (Adult < 0)
      {
         cout << "Cannot accept negative seat numbers" << endl;
         cin.clear();
         cin.ignore(23000, '\n');
      }

      cout << "How many adult seats?" << endl;
      cin >> Adult;
   }while (Adult < 0);

   //receive and validate Child ticket number
   do
   {
      if (Child < 0)
      {
         cout << "Cannot accept negative seat numbers" << endl;
         cin.clear();
         cin.ignore(23000, '\n');
      }

      cout << "How many child seats?" << endl;
      cin >> Child;
   }while (Child < 0);

   //receive and validate Senior ticket number
   do
   {
      if (Senior < 0)
      {
         cout << "Cannot accept negative seat numbers" << endl;
         cin.clear();
         cin.ignore(23000, '\n');
      }

      cout << "How many senior seats?" << endl;
      cin >> Senior;
   }while (Senior < 0);


   //confirm that these seats fit in one row
   if (Adult + Child + Senior > Length)
   {
      cout << "There are too many seats to enter into one row" << endl
         << "Please enter a valid number of seats" << endl << endl;
      SeatInput(Audit, Row, Seat, Adult, Child, Senior, Length);
   }
}



//this function returns true if the seats are available
bool Available(node* ptr, int Row, char Seat, int TotalSeats, int Length)
{

   //position pointer at the beginning of the selected seats
   while (ptr->row != Row)
      ptr = ptr->Down;

   while (ptr->seat != Seat)
      ptr = ptr->Right;

   //check the availability of each seat
   for (int i = 0; i < TotalSeats; i++)
   {
      if (ptr == nullptr)
         return false;

      if (ptr->reserved == true)
         return false;

      ptr = ptr->Right;
   }

   //return true if nothing other than open spaces were detected
   return true;
}



//this function writes the reserved seats to the file
void ReserveSeats(fstream& Audit, node* ptr, int Row, char Seat, int Adult, int Child, int Senior, int Length)
{

   //position file pointer in front of the seats to be reserved
   Audit.seekp((Row-1)*(Length+2)+ Seat - 'A', ios::beg);

   //position pointer at the beginning of the selected seats
   while (ptr->row != Row)
      ptr = ptr->Down;

   while (ptr->seat != Seat)
      ptr = ptr->Right;

   //display adult seats
   for (int i = 0; i < Adult; i++)
   {
      Audit.put('A');
      ptr->type = 'A';
      ptr->reserved = true;
      ptr=ptr->Right;
   }

   //display child seats
   for (int i = 0; i < Child; i++)
   {
      Audit.put('C');
      ptr->type = 'C';
      ptr->reserved = true;
      ptr=ptr->Right;
   }

   //display senior seats
   for (int i = 0; i < Senior; i++)
   {
      Audit.put('S');
      ptr->type = 'S';
      ptr->reserved = true;
      ptr=ptr->Right;
   }

   //indicate seats have been reserved
   cout << "Your seats have been reserved" << endl << endl;
}



//this function finds alternate seats and prompts the user to accept or decline
node* BestSeats(fstream& Audit, node* ptr, int Adult, int Child, int Senior, int Length)
{
   int TotalSeats = Adult + Child + Senior;
   int Rows = AmountRows(Audit, Length);
   double CenterRow = (Rows + 1)/2.0;
   double CenterSeat = ('A' + Length + 'A')/2.0;
   node* rowhead = ptr;
   const double HALFSELECT = TotalSeats/2.0;
   node* BestSeat = nullptr;
   int BestDist = Length + Rows;
   int pivot;
   int pivotdist = 1;

   if (Rows % 2 == 0)
      pivot = 1;
   else
      pivot = -1;

   for (int i = 1; i <= CenterRow - 1; i++)
   {
      rowhead = rowhead->Down;
      ptr = ptr->Down;
   }

   double distance;

   do
   {
      for (int counter = 0; counter <= Length - TotalSeats; counter++)
      {
         distance = sqrt( pow(CenterRow - ptr->row, 2.0) + pow(CenterSeat - (ptr->seat + HALFSELECT) , 2.0) );

         if (Available(ptr, ptr->row, ptr->seat, TotalSeats, Length) && distance < BestDist)
         {
            BestSeat = ptr;
            BestDist = distance;
         }

         ptr = ptr->Right;
      }

      for (int i = 0; i < pivotdist; i++)
      {
         if (pivot < 0)
            rowhead = rowhead->Up;
         if (pivot > 0)
            rowhead = rowhead->Down;
      }

      ptr = rowhead;
      pivot *= -1;
      pivotdist += 1;

   } while (ptr != nullptr);


   return BestSeat;
}



//determines the amount of a character in the linked grid
int Tabulate (fstream& File, node* head, int Length, char input)
{
   int rows = AmountRows(File, Length);
   node* rowhead = head;
   int counter = 0;


   //step through each element of the grid
   for (int r = 1; r <= rows; r++)
   {
      for (char i = 'A'; i < 'A' + Length; i++)
      {
        if (toupper(head->type) == input)
         counter++;

        head=head->Right;
      }

      rowhead = rowhead->Down;
      head = rowhead;
   }

   return counter;
}
