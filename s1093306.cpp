#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <vector>
#include <string>
using namespace::std;

struct Date
{
    int year = 2000;
    int month = 0;
    int day = 0;
};

struct Reservation
{
    char phoneNumber[12] = ""; // phone number
    char name[8] = "";         // name
    Date date = Date();          // reservation date
    int time = 0;                // reservation time：1 for 11:30, 2 for 13:30,
                                 //                   3 for 17:45, 4 for 19:45
    int partySize = 0;           // the number of customers for a reservation
    int menu = 0;                // 1 for NT$ 1080, 2 for NT$ 1680, 3 for NT$ 2280
};

struct AvailSeats
{
    Date date = Date(); // a date
    int numAvailSeats[5] = {};
}; // numAvailSeats[ 1 ]: the number of seats available for lunch #1 (11:30 ~ 13:30)
   // numAvailSeats[ 2 ]: the number of seats available for lunch #2 (13:30 ~ 15:30)
   // numAvailSeats[ 3 ]: the number of seats available for dinner #1 (17:45 ~ 19:45)
   // numAvailSeats[ 4 ]: the number of seats available for dinner #2 (19:45 ~ 21:45)

const int totalNumSeats = 15;

// array of days per month
int days[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// loads reservations from the file Reservations.dat
void loadReservations(vector< Reservation >& reservations);

// loads availSeats from the file AvailSeats.dat
void loadAvailSeats(vector< AvailSeats >& availSeats);

// initialize availSeats
void initializeAvailSeats(vector< AvailSeats >& availSeats);

// returns true if and only if date1 is earlier than date2
bool earlier(const Date& date1, const Date& date2);

// returns date1 - date2, i.e., the number of days between date1 and date2
int difference(const Date& date1, const Date& date2);

// input an integer in the range [ begin, end ]
int inputAnInteger(int begin, int end);

// make a reservation
void makeReservation(vector< Reservation >& reservations,
    vector< AvailSeats >& availSeats);

// compute the current date
void computeCurrentDate(Date& currentDate);

// returns true if and only if the specified year is a leap year
bool leapYear(int year);

// input reservation date
void inputDate(vector< AvailSeats >& availSeats, Date& date,
    const Date& currentDate, int partySize);

// returns currentDate + numDays
Date addition(const Date& currentDate, int numDays);

// returns true if and only if there are at least n seats available for date,
// where n = requiredSeats
bool hasAvailableSeats(const vector< AvailSeats >& availSeats, const Date date,
    int requiredSeats);

// returns true if availSeats[ i ].numAvailSeats[ timeCode ] >= requiredSeats,
// where availSeats[ i ].date == date
bool hasAvailableSeats(const vector< AvailSeats >& availSeats, const Date date,
    int timeCode, int requiredSeats);

// returns a nonnegative integer i if availSeats[ i ].date == date,
// returns -1 otherwise.
int findAvailSeat(const vector< AvailSeats >& availSeats, Date date);

// choose a reservation time
void inputTimeCode(const vector< AvailSeats >& availSeats, const Date date,
    int& timeCode, int partySize);

// displays partySize, date and time in reservation
void displayReservationInfo(const Reservation& reservation);

// availSeats[ i ].numAvailSeats[ timeCode ] -= requiredSeats,
// where availSeats[ i ].date == date
void decreaseAvailSeats(vector< AvailSeats >& availSeats, const Date date,
    int timeCode, int requiredSeats);

// input a phoneNumber,
// then displays partySize, date and time in all reservations for phoneNumber
void viewReservation(const vector< Reservation >& reservations);

// returns true if and only if there is an i such that
// reservations[ i ].phoneNumber == phoneNumber
bool exist(const vector< Reservation >& reservations, char phoneNumber[]);

// displays partySize, date and time in all reservations for phoneNumber
void displayReservationInfo(const vector< Reservation >& reservations,
    char phoneNumber[]);

// stores reservations into the file Reservations.dat
void storeReservations(vector< Reservation >& reservations);

// stores availSeats into the file AvailSeats.dat
void storeAvailSeats(vector< AvailSeats >& availSeats);

int main()
{
    cout << "Welcome to Zuo Zhe Zuo Sushi Wo Shou Si\n";

    vector< Reservation > reservations; // vector of all reservations
    vector< AvailSeats > availSeats;    // Vector of available seats

    loadReservations(reservations);
    initializeAvailSeats(availSeats);

    int choice;
    while (true)
    {
        cout << "\nEnter Your Choice\n"
            << "1. Make Reservation\n"
            << "2. Reservation Enquiry\n"
            << "3. End Program\n";

        do cout << "? ";
        while ((choice = inputAnInteger(1, 3)) == -1);

        switch (choice)
        {
        case 1:
            makeReservation(reservations, availSeats);
            break;
        case 2:
            viewReservation(reservations);
            break;
        case 3:
            cout << "\nThank you. Goodbye.\n\n";
            storeReservations(reservations);
            storeAvailSeats(availSeats);
            system("pause");
            return 0;
        default:
            cerr << "Incorrect Choice!\n";
            break;
        }
    }

    system("pause");
}

void loadReservations(vector< Reservation >& reservations)
{
    ifstream inFile("Reservations.dat", ios::in | ios::binary);
    if (!inFile)
    {
        cout << "File could not be opened";
        system("pause");
        exit(1);
    }

    Reservation inputTemp;

    inFile.seekg(0, ios::end);
    int size = inFile.tellg() / sizeof(Reservation);
    inFile.seekg(0, ios::beg);

    for (int i = 0; i < size; i++)
    {
        inFile.read(reinterpret_cast<char*>(&inputTemp), sizeof(Reservation));
        reservations.push_back(inputTemp);
    }

    inFile.close();
}

void loadAvailSeats(vector< AvailSeats >& availSeats)
{
    ifstream inFile("AvailSeats.dat", ios::in | ios::binary);
    if (!inFile)
    {
        cout << "File could not be opened";
        system("pause");
        exit(1);
    }

    AvailSeats inputTemp;

    inFile.seekg(0, ios::end);
    int size = inFile.tellg() / sizeof(AvailSeats);
    inFile.seekg(0, ios::beg);

    for (int i = 0; i < size; i++)
    {
        inFile.read(reinterpret_cast<char*>(&inputTemp), sizeof(AvailSeats));
        availSeats.push_back(inputTemp);
    }

    inFile.close();
}

void initializeAvailSeats(vector< AvailSeats >& availSeats)
{
    Date currentDate;
    computeCurrentDate(currentDate);

    loadAvailSeats(availSeats);

    Date date;
    if (availSeats.size() == 0)
    {
        AvailSeats newAvailSeat;
        for (int i = 0; i < 30; i++)
        {
            newAvailSeat.date = addition(currentDate, i + 1);
            newAvailSeat.numAvailSeats[1] = totalNumSeats;
            newAvailSeat.numAvailSeats[2] = totalNumSeats;
            newAvailSeat.numAvailSeats[3] = totalNumSeats;
            newAvailSeat.numAvailSeats[4] = totalNumSeats;
            availSeats.push_back(newAvailSeat);
        }
    }
    else
    {
        if (earlier(availSeats[availSeats.size() - 1].date, currentDate))
        {
            if (availSeats[availSeats.size() - 1].date.year == currentDate.year &&
                availSeats[availSeats.size() - 1].date.month == currentDate.month &&
                availSeats[availSeats.size() - 1].date.day == currentDate.day)  //今天為原始資料的最後一天 -> 保留最後一天原始資料

            {
                availSeats[0] = availSeats[availSeats.size() - 1];  //保留最後一天原始資料
                availSeats.resize(availSeats.size() + 1);  //調整長度

                AvailSeats newAvailSeat;
                for (int i = 0; i < 30; i++)
                {
                    newAvailSeat.date = addition(currentDate, i + 1);
                    newAvailSeat.numAvailSeats[1] = totalNumSeats;
                    newAvailSeat.numAvailSeats[2] = totalNumSeats;
                    newAvailSeat.numAvailSeats[3] = totalNumSeats;
                    newAvailSeat.numAvailSeats[4] = totalNumSeats;
                    availSeats[i + 1] = newAvailSeat;
                }
            }
            else  //全覆蓋
            {
                AvailSeats newAvailSeat;
                for (int i = 0; i < 30; i++)
                {
                    newAvailSeat.date = addition(currentDate, i + 1);
                    newAvailSeat.numAvailSeats[1] = totalNumSeats;
                    newAvailSeat.numAvailSeats[2] = totalNumSeats;
                    newAvailSeat.numAvailSeats[3] = totalNumSeats;
                    newAvailSeat.numAvailSeats[4] = totalNumSeats;
                    availSeats[i] = newAvailSeat;
                }
            }
        }
        else if (earlier(availSeats[0].date, currentDate))
        {
            if (availSeats[0].date.year == currentDate.year &&
                availSeats[0].date.month == currentDate.month &&
                availSeats[0].date.day == currentDate.day)  //今天為原始資料的第一天 -> 保留原始資料 and 原始資料的最後一天要再+1
            {
                availSeats.resize(availSeats.size() + 1);
                availSeats[availSeats.size() - 1].date = addition(availSeats[availSeats.size() - 2].date, 1);
                availSeats[availSeats.size() - 1].numAvailSeats[1] = totalNumSeats;
                availSeats[availSeats.size() - 1].numAvailSeats[2] = totalNumSeats;
                availSeats[availSeats.size() - 1].numAvailSeats[3] = totalNumSeats;
                availSeats[availSeats.size() - 1].numAvailSeats[4] = totalNumSeats;
            }
            else  //今天為原始資料中間的某天 -> 保留日期為今天以後的原始資料 and 增加資料到31天
            {
                int todayPos = difference(availSeats[0].date, currentDate);

                vector<AvailSeats> temp;
                for (int i = todayPos; i < availSeats.size(); i++)  //保留原始資料
                    temp.push_back(availSeats[i]);
                while (temp.size() < 31)  //增加資料到31天
                {
                    temp.resize(availSeats.size() + 1);
                    temp[temp.size() - 1].date = addition(temp[temp.size() - 2].date, 1);
                    temp[temp.size() - 1].numAvailSeats[1] = totalNumSeats;
                    temp[temp.size() - 1].numAvailSeats[2] = totalNumSeats;
                    temp[temp.size() - 1].numAvailSeats[3] = totalNumSeats;
                    temp[temp.size() - 1].numAvailSeats[4] = totalNumSeats;
                }

                //全覆蓋
                availSeats.resize(temp.size());
                for (int i = 0; i < temp.size(); i++)
                    availSeats[i] = temp[i];
            }
        }
    }
}

bool earlier(const Date& date1, const Date& date2)  //return true if date1 <= date2
{
    if (date1.year > date2.year)
        return false;
    else if (date1.year == date2.year)
    {
        if (date1.month > date2.month)
            return false;
        else if (date1.month == date2.month)
        {
            if (date1.day <= date2.day)
                return true;
            else
                return false;
        }
        else
            return true;
    }
    else
        return true;
}

int difference(const Date& date1, const Date& date2)
{
    if (earlier(date1, date2))  //date1 <= date2
    {
        int diff = 0;
        Date sum = date1;
        while (sum.year != date2.year ||
            sum.month != date2.month ||
            sum.day != date2.day)
        {
            addition(sum, diff);
            diff++;
        }
        return diff;
    }
    else
    {
        int diff = 0;
        Date sum = date2;
        while (sum.year != date1.year ||
            sum.month != date1.month ||
            sum.day != date1.day)
        {
            addition(sum, diff);
            diff++;
        }
        return diff;
    }
}

int inputAnInteger(int begin, int end)
{
    char inputTemp[100];
    int returnNum;
    cin.getline(inputTemp, 100);
    for (size_t i = 0; i < strlen(inputTemp); i++)
        if (inputTemp[i] < '0' || inputTemp[i] > '9')
            return -1;

    returnNum = atoi(inputTemp);
    return (returnNum >= begin && returnNum <= end) ? returnNum : -1;
}

void makeReservation(vector< Reservation >& reservations,
    vector< AvailSeats >& availSeats)
{
    Reservation newReservation;
    cout << endl;
    do cout << "Enter the party size (1~6): ";
    while ((newReservation.partySize = inputAnInteger(1, 6)) == -1);

    Date currentDate;
    computeCurrentDate(currentDate);

    inputDate(availSeats, newReservation.date, currentDate,
        newReservation.partySize);

    inputTimeCode(availSeats, newReservation.date, newReservation.time,
        newReservation.partySize);

    cout << "\nEnter name: ";
    cin >> newReservation.name;

    cout << "\nEnter phone Number: ";
    cin >> newReservation.phoneNumber;
    cin.ignore();

    cout << "\nChoose a menu:\n";
    cout << "1. NT$ 1080\n";
    cout << "2. NT$ 1680\n";
    cout << "3. NT$ 2280\n";

    do cout << "? ";
    while ((newReservation.menu = inputAnInteger(1, 3)) == -1);

    cout << endl;
    displayReservationInfo(newReservation);

    cout << "\nReservation Completed.\n";

    reservations.push_back(newReservation);

    decreaseAvailSeats(availSeats, newReservation.date, newReservation.time,
        newReservation.partySize);
}

void computeCurrentDate(Date& currentDate)
{
    int remainingSeconds = static_cast<int>(time(0)) + 8 * 60 * 60;

    currentDate.year = 1970;

    int secondsPerYear = 365 * 24 * 60 * 60;
    if (leapYear(currentDate.year))
        secondsPerYear += 24 * 60 * 60;

    while (remainingSeconds >= secondsPerYear)
    {
        remainingSeconds -= secondsPerYear;
        currentDate.year++;

        secondsPerYear = 365 * 24 * 60 * 60;
        if (leapYear(currentDate.year))
            secondsPerYear += 24 * 60 * 60;
    }

    int days[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (leapYear(currentDate.year))
        days[2] = 29;

    currentDate.month = 1;
    while (remainingSeconds >= days[currentDate.month] * 24 * 60 * 60)
    {
        remainingSeconds -= days[currentDate.month] * 24 * 60 * 60;
        currentDate.month++;
    }

    currentDate.day = remainingSeconds / (24 * 60 * 60) + 1;
}

bool leapYear(int year)
{
    return (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0));
}

void inputDate(vector< AvailSeats >& availSeats, Date& date,
    const Date& currentDate, int partySize)
{
    int endlControl = 0;
    int beg = (availSeats.size() == 31) ? 1 : 0;
    cout << "\nChoose a date\n";
    for (int i = 1; i <= 30; i++)
    {
        Date today = addition(currentDate, i);
        if (hasAvailableSeats(availSeats, today, partySize))
        {
            cout << setw(2) << setfill(' ') << i << ". ";
            cout << availSeats[i - 1 + beg].date.year << '/' << setw(2) << setfill('0') << availSeats[i - 1 + beg].date.month << '/' << setw(2) << availSeats[i - 1 + beg].date.day;
            endlControl++;
        }

        if (endlControl == 4)
        {
            cout << endl;
            endlControl = 0;
        }
        else if (hasAvailableSeats(availSeats, today, partySize))
            cout << "  ";
    }
    cout << endl;

    int choice;
    do cout << "? ";
    while ((choice = inputAnInteger(1, 30)) == -1 || !hasAvailableSeats(availSeats, availSeats[choice - 1 + beg].date, partySize));

    //放入newReservation
    date = availSeats[choice - 1].date;
}

Date addition(const Date& currentDate, int numDays)
{
    Date sum;
    sum.day = currentDate.day + numDays;
    sum.month = currentDate.month;
    sum.year = currentDate.year;

    if (sum.day > 0)
    {
        while (sum.day > days[sum.month])
        {
            if (sum.month == 2 && leapYear(sum.year))
                days[2] = 29;
            sum.day -= days[sum.month];
            sum.month++;
            if (sum.month > 12)
            {
                sum.year++;
                sum.month = 1;
            }
        }
        days[2] = 28;
    }
    else
    {
        while (sum.day <= 0)
        {
            if (sum.month == 2 && leapYear(sum.year))
                days[2] = 29;
            sum.month--;
            if (sum.month == 0)
            {
                sum.year--;
                sum.month = 12;
            }
            sum.day += days[sum.month];
        }
        days[2] = 28;
    }

    return sum;
}

bool hasAvailableSeats(const vector< AvailSeats >& availSeats, const Date date,
    int requiredSeats)
{
    int pos = findAvailSeat(availSeats, date);
    if (pos == -1)
        return false;
    if (availSeats[pos].numAvailSeats[1] >= requiredSeats || availSeats[pos].numAvailSeats[2] >= requiredSeats ||
        availSeats[pos].numAvailSeats[3] >= requiredSeats || availSeats[pos].numAvailSeats[4] >= requiredSeats)
        return true;
    else
        return false;
}

bool hasAvailableSeats(const vector< AvailSeats >& availSeats, const Date date,
    int timeCode, int requiredSeats)
{
    int pos = findAvailSeat(availSeats, date);
    if (pos == -1)
        return false;
    if (availSeats[pos].numAvailSeats[timeCode] >= requiredSeats)
        return true;
    else
        return false;
}

int findAvailSeat(const vector< AvailSeats >& availSeats, Date date)
{
    int pos = (availSeats.size() == 31) ? 1 : 0;
    while (pos < availSeats.size())
    {
        if (availSeats[pos].date.year == date.year && availSeats[pos].date.month == date.month && availSeats[pos].date.day == date.day)
            return pos;
        pos++;
    }
    return -1;
}

void inputTimeCode(const vector< AvailSeats >& availSeats, const Date date,
    int& timeCode, int partySize)
{
    int recordDate = (availSeats.size() == 31) ? 1 : 0;
    bool canChoice[4] = {};
    for (int i = 0; i < availSeats.size(); i++)
        if (availSeats[i].date.year == date.year && availSeats[i].date.month == date.month && availSeats[i].date.day == date.day)
        {
            recordDate += i;
            break;
        }
    cout << "\nChoose a time:\n";
    if (availSeats[recordDate].numAvailSeats[1] >= partySize)
    {
        cout << "1. 11:30" << endl;
        canChoice[0] = true;
    }
    if (availSeats[recordDate].numAvailSeats[2] >= partySize)
    {
        cout << "2. 13:30" << endl;
        canChoice[1] = true;
    }
    if (availSeats[recordDate].numAvailSeats[3] >= partySize)
    {
        cout << "3. 17:45" << endl;
        canChoice[2] = true;
    }
    if (availSeats[recordDate].numAvailSeats[4] >= partySize)
    {
        cout << "4. 19:45" << endl;
        canChoice[3] = true;
    }

    int choice;
    do cout << "? ";
    while (((choice = inputAnInteger(1, 4)) == -1) || !canChoice[choice - 1]);

    //放入newReservation
    timeCode = choice;
}

void displayReservationInfo(const Reservation& reservation)
{
    cout << reservation.partySize << " guests  ";

    cout << reservation.date.year << "/";

    if (reservation.date.month < 10)
        cout << '0';
    cout << reservation.date.month << "/";

    if (reservation.date.day < 10)
        cout << '0';
    cout << reservation.date.day;

    char times[5][8] = { "", "11:30", "13:30", "17:45", "19:45" };
    cout << "  " << times[reservation.time] << endl;
}

void decreaseAvailSeats(vector< AvailSeats >& availSeats, const Date date,
    int timeCode, int requiredSeats)
{
    int pos = findAvailSeat(availSeats, date);
    if (pos >= 0) // found
        availSeats[pos].numAvailSeats[timeCode] -= requiredSeats;
}

void viewReservation(const vector< Reservation >& reservations)
{
    if (reservations.size() == 0)
    {
        cout << "\nNo reservations!\n";
        return;
    }

    char phoneNumber[12];
    cout << "\nEnter phone Number: ";
    cin >> phoneNumber;
    cin.ignore();

    if (!exist(reservations, phoneNumber))
    {
        cout << "\nYou have no reservations!\n";
        return;
    }

    displayReservationInfo(reservations, phoneNumber);
}

bool exist(const vector< Reservation >& reservations, char phoneNumber[])
{
    for (int i = 0; i < reservations.size(); i++)
        if (strcmp(reservations[i].phoneNumber, phoneNumber) == 0)
            return true;
    return false;
}

void displayReservationInfo(const vector< Reservation >& reservations,
    char phoneNumber[])
{
    cout << endl;
    int count = 0;
    for (size_t i = 0; i < reservations.size(); ++i)
        if (strcmp(reservations[i].phoneNumber, phoneNumber) == 0)
        {
            cout << setw(2) << ++count << ". ";
            displayReservationInfo(reservations[i]);
        }
}

void storeReservations(vector< Reservation >& reservations)
{
    ofstream outFile("Reservations.dat", ios::out | ios::binary);

    for (int i = 0; i < reservations.size(); i++)
        outFile.write(reinterpret_cast<const char*>(&reservations[i]), sizeof(Reservation));

    outFile.close();
}

void storeAvailSeats(vector< AvailSeats >& availSeats)
{
    ofstream outFile("AvailSeats.dat", ios::out | ios::binary);

    for (int i = 0; i < availSeats.size(); i++)
        outFile.write(reinterpret_cast<const char*>(&availSeats[i]), sizeof(AvailSeats));

    outFile.close();
}
