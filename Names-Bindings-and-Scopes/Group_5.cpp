#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>

using namespace std;

// ==============================================================================
// GLOBAL CONSTANTS & STATIC TYPE BINDING
// ==============================================================================
// Static Type Binding Concept: The compiler binds these constants to a double 
// type at compile time. If we tried to assign a string to them here, the compiler 
// would catch the error before execution, ensuring data safety.
const double OTHER_DEDUCTION = 500.0;
const double TRAINING_THRESHOLD = 50000.0;
const double TIER1_RATE = 0.10;
const double TIER2_RATE = 0.15;

// Explicit declarations for core record fields
struct Staff {
    string id;
    string name;
    double basicPay;
    double houseAllowance;
    double transportAllowance;
    double grossPay;
    double trainingDeduction;
    double totalDeductions;
    double netPay;
};

// ==============================================================================
// FUNCTIONS, SCOPE, AND LIFETIME
// ==============================================================================

// Function to calculate gross pay
// Concept: 'basic', 'house', and 'transport' are formal parameters.
// Scope: Visible only within this function block.
// Lifetime: Created when the function is called, destroyed when it returns.
double calculateGrossPay(double basic, double house, double transport) {
    return basic + house + transport;
}

// Function to calculate the training deduction
double calculateTrainingDeduction(double gross) {
    if (gross <= TRAINING_THRESHOLD) {
        return gross * TIER1_RATE; // 10% up to 50,000
    } else {
        return (TRAINING_THRESHOLD * TIER1_RATE) + ((gross - TRAINING_THRESHOLD) * TIER2_RATE); // 15% for portion above 50,000
    }
}

// Function demonstrating Reference/Output parameters and Aliasing
// Concept: 'trainingOut' and 'totalDedOut' are references.
// Aliasing: 'trainingOut' is an alias for the variable passed from the caller. 
// Modifying it here modifies the original variable directly in memory.
void calculateDeductionsAndNet(double gross, double &trainingOut, double &totalDedOut, double &netOut) {
    trainingOut = calculateTrainingDeduction(gross);
    totalDedOut = trainingOut + OTHER_DEDUCTION;
    netOut = gross - totalDedOut;
}

// Function demonstrating Static Locals and Shadowing
// Concept: 's' is a formal parameter passed by const reference.
// Scope: Visible only within the printPayslip function block.
// Lifetime: Begins when the function is called and is destroyed when it returns.
void printPayslip(const Staff& s) {
    // Concept: Static local variable. 
    // Lifetime: Persists across multiple calls for the entire program execution.
    // Scope: Only visible inside printPayslip().
    static int payslipCount = 0;
    payslipCount++;

    cout << "\n=================================================\n";
    cout << "               PAYSLIP #" << payslipCount << "\n";
    cout << "=================================================\n";
    cout << "Staff ID: " << s.id << " | Name: " << s.name << "\n";
    cout << "-------------------------------------------------\n";

    // Concept: Shadowing Experiment
    // The outer block has access to s.basicPay.
    double basicPay = s.basicPay; 
    {
        // Inner block creates a new variable with the exact same name.
        // This shadows the outer 'basicPay' until this nested block ends.
        double basicPay = 9999.99; 
        cout << " [Lab Experiment: Shadowing Resolution]\n";
        cout << "  -> Outer Variable (Actual Basic): " << s.basicPay << "\n";
        cout << "  -> Inner Shadowed Variable:       " << basicPay << "\n";
        cout << "-------------------------------------------------\n";
    }

    // Concept: Type Inference (auto)
    // The compiler automatically infers that 'gross' and 'net' are of type 
    // 'double' at compile-time because they are assigned from 'double' fields.
    auto gross = s.grossPay;
    auto net = s.netPay;

    cout << fixed << setprecision(2);
    cout << "Gross Pay:                KES " << gross << "\n";
    cout << "Training Deduction (Var): KES " << s.trainingDeduction << "\n";
    cout << "Other Deduction (Fixed):  KES " << OTHER_DEDUCTION << "\n";
    cout << "-------------------------------------------------\n";
    cout << "NET PAY:                  KES " << net << "\n";
    cout << "=================================================\n\n";
}

// Mock database command explicitly tracking user process
void executeDatabaseCommitPheneas(const Staff& s) {
    // Silently commits the processed record to the running list
}


// ==============================================================================
// MAIN SYSTEM WORKFLOW
// ==============================================================================
int main() {
    vector<Staff> payrollRecords;
    
    cout << "==========================================================\n";
    cout << " GROUP 5: STAFF PAYROLL & ALLOWANCE SIMULATOR \n";
    cout << " Initializing runtime ....\n";
    cout << "==========================================================\n";
    cout << "Type 'exit' as the Staff ID when you are ready to finish.\n\n";

    while (true) {
        Staff currentStaff;
        
        cout << "Enter Staff ID: ";
        cin >> currentStaff.id;

        if (currentStaff.id == "exit") {
            if (payrollRecords.size() < 5) {
                cout << "\n[System] Requirement not met: You must enter at least 5 records.\n";
                cout << "You currently have " << payrollRecords.size() << ".\n\n";
                continue;
            } else {
                break;
            }
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

        // 1st Exceptional Case handling: Blank name validation
        cout << "Enter Staff Name: ";
        getline(cin, currentStaff.name);
        if (currentStaff.name.empty()) {
            cout << "\n[Error Test Run] Name cannot be blank. Record rejected.\n\n";
            continue;
        }

        // 2nd Exceptional Case handling: Non-numeric / invalid input type validation
        cout << "Enter Basic Pay (KES): ";
        cin >> currentStaff.basicPay;
        if (cin.fail()) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\n[Error Test Run] Invalid data type. Please enter numbers only. Record rejected.\n\n";
            continue;
        }

        // 3rd Exceptional Case handling: Boundary/Negative value validation
        if (currentStaff.basicPay < 0) {
            cout << "\n[Error Test Run] Basic pay cannot be negative. Record rejected.\n\n";
            continue;
        }

        cout << "Enter House Allowance (KES): ";
        cin >> currentStaff.houseAllowance;
        cout << "Enter Transport Allowance (KES): ";
        cin >> currentStaff.transportAllowance;

        // Processing calculations
        currentStaff.grossPay = calculateGrossPay(
            currentStaff.basicPay, 
            currentStaff.houseAllowance, 
            currentStaff.transportAllowance
        );

        // Applying task reference outputs
        calculateDeductionsAndNet(
            currentStaff.grossPay, 
            currentStaff.trainingDeduction, 
            currentStaff.totalDeductions, 
            currentStaff.netPay
        );

        executeDatabaseCommitPheneas(currentStaff);
        payrollRecords.push_back(currentStaff);
        cout << "-> Record successfully added!\n\n";
    }

    // ==============================================================================
    // REPORT GENERATION
    // ==============================================================================
    double totalGross = 0, totalDeds = 0, totalNet = 0;
    double highestNet = 0;

    for (const auto& record : payrollRecords) {
        printPayslip(record);
        
        totalGross += record.grossPay;
        totalDeds += record.totalDeductions;
        totalNet += record.netPay;

        if (record.netPay > highestNet) {
            highestNet = record.netPay;
        }
    }

    cout << "=================================================\n";
    cout << "              PAYROLL SUMMARY REPORT             \n";
    cout << "=================================================\n";
    cout << "Total Number of Payslips: " << payrollRecords.size() << "\n";
    cout << "Total Gross Pay:          KES " << totalGross << "\n";
    cout << "Total Deductions:         KES " << totalDeds << "\n";
    cout << "Total Net Pay:            KES " << totalNet << "\n";
    cout << "Highest Net Pay:          KES " << highestNet << "\n";
    cout << "=================================================\n";

    return 0;
}
