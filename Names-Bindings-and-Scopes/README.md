# 💼 Staff Payroll & Allowance Processing Simulator

### CCS 2105 — Programming Languages Lab: Names, Bindings and Scopes

---

## 👥 Group 5 Members

| # | Name | Registration Number | Contribution |
|---|------|---------------------|-------------|
| 1 | **Pheneas Lazarus** | C026-01-0986/2025 | System architecture, struct design, aliasing & reference parameter logic |
| 2 | **Timon Kandie** | C026-01-0956/2025 | Input validation, error handling, main workflow loop, testing |
| 3 | **Victor Rutto** | C026-01-0960/2025 | Shadowing experiment, report generation, documentation |

---

## 📖 System Documentation

### 1. Problem Statement

Programming language concepts such as **names**, **bindings**, **scopes**, **aliasing**, **shadowing**, and **type inference** are fundamental to understanding how compilers and interpreters manage variables, memory, and program structure. However, these concepts are often taught in isolation without a practical, integrated demonstration.

This project implements a **Staff Payroll & Allowance Processing Simulator** in C++ that serves as a practical laboratory for demonstrating these core programming language concepts within a real-world payroll computation scenario.

### 2. Objectives

- Demonstrate **static type binding** using compile-time constant declarations
- Show **explicit declarations** through a well-defined `Staff` struct
- Illustrate **variable scope and lifetime** through function parameters and local variables
- Implement **aliasing** via C++ reference (output) parameters
- Create a deliberate **variable shadowing** experiment using nested blocks
- Utilize **type inference** with the C++ `auto` keyword
- Demonstrate **static local variables** with persistent lifetime across function calls
- Implement robust **input validation** covering three exceptional cases

### 3. System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│           GLOBAL CONSTANTS (Static Type Binding)            │
│   OTHER_DEDUCTION, TRAINING_THRESHOLD, TIER1/2_RATE         │
└──────────────────────────┬──────────────────────────────────┘
                          │
              ┌───────────┴───────────┐
              │   Staff Struct            │
              │   (Explicit Declaration)   │
              │   id, name, basicPay,      │
              │   houseAllowance, etc.     │
              └───────────┬───────────┘
                          │
         ┌──────────────┼──────────────┐
         │              │              │
┌────────┴──────┐ ┌────┴─────────┐ ┌────┴────────┐
│ calculateGross-│ │ calculateDed-│ │ printPayslip │
│ Pay()          │ │ uctionsAnd-  │ │              │
│ (Scope/       │ │ Net()        │ │ (Shadowing,  │
│  Lifetime)    │ │ (Aliasing/   │ │  Static Var, │
│               │ │  References) │ │  auto/Type   │
└───────────────┘ └──────────────┘ │  Inference)  │
                                  └─────────────┘
```

### 4. Key Concepts Demonstrated

#### 4.1 Static Type Binding (Global Constants)

The compiler binds these constants to the `double` type at **compile time**. If we attempted to assign a string value to them, the compiler would catch the error before execution, ensuring data safety.

```cpp
const double OTHER_DEDUCTION = 500.0;
const double TRAINING_THRESHOLD = 50000.0;
const double TIER1_RATE = 0.10;
const double TIER2_RATE = 0.15;
```

**Concept**: The binding between the name `OTHER_DEDUCTION` and its type `double` is established statically (at compile time) and cannot change during execution.

#### 4.2 Explicit Declarations (Staff Struct)

All record fields are explicitly declared with their types in the `Staff` struct. The programmer must state the type of each field, leaving no ambiguity for the compiler:

```cpp
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
```

#### 4.3 Variable Scope & Lifetime (Function Parameters)

The parameters `basic`, `house`, and `transport` are **formal parameters** whose:
- **Scope**: Visible only within the `calculateGrossPay` function block
- **Lifetime**: Created when the function is called, destroyed when it returns

```cpp
double calculateGrossPay(double basic, double house, double transport) {
    return basic + house + transport;
}
```

#### 4.4 Aliasing via Reference Parameters

The `calculateDeductionsAndNet` function uses **reference parameters** (`&`), creating **aliases** for the caller’s variables. Modifying `trainingOut` inside the function directly modifies the original variable in the caller’s memory:

```cpp
void calculateDeductionsAndNet(double gross, double &trainingOut, 
                                double &totalDedOut, double &netOut) {
    trainingOut = calculateTrainingDeduction(gross);
    totalDedOut = trainingOut + OTHER_DEDUCTION;
    netOut = gross - totalDedOut;
}
```

**Concept**: `trainingOut` and `currentStaff.trainingDeduction` refer to the **same memory location**. This is aliasing — two different names bound to the same address.

#### 4.5 Variable Shadowing (Nested Block Experiment)

A deliberate experiment where an inner block declares a variable with the **same name** as an outer block variable. The inner `basicPay` **shadows** the outer one within its scope:

```cpp
void printPayslip(const Staff& s) {
    double basicPay = s.basicPay;       // Outer variable
    {
        double basicPay = 9999.99;      // Inner variable SHADOWS outer
        cout << "Outer Variable (Actual Basic): " << s.basicPay << endl;
        cout << "Inner Shadowed Variable:       " << basicPay << endl;
    }
    // Outer 'basicPay' is accessible again here
}
```

**Output** demonstrates the compiler resolves the inner `basicPay` to `9999.99` while the outer remains unchanged at the actual staff basic pay.

#### 4.6 Type Inference (`auto` keyword)

The C++ `auto` keyword lets the compiler **automatically infer** the variable type from the assigned expression at compile time:

```cpp
auto gross = s.grossPay;   // Compiler infers: double
auto net = s.netPay;       // Compiler infers: double
```

**Concept**: This is still static type binding — the type is determined at compile time, not runtime. The programmer simply omits the explicit type declaration.

#### 4.7 Static Local Variables

The `payslipCount` variable inside `printPayslip()` is declared `static`, giving it a **persistent lifetime** that spans the entire program execution, while its **scope** remains local to the function:

```cpp
void printPayslip(const Staff& s) {
    static int payslipCount = 0;   // Persists across calls
    payslipCount++;
    cout << "PAYSLIP #" << payslipCount << endl;
}
```

**Concept**: Unlike regular local variables (which are created and destroyed with each call), `payslipCount` retains its value between function invocations, enabling automatic payslip numbering.

### 5. Input Validation & Error Handling

The system tests **three exceptional cases** to demonstrate robustness:

| # | Error Case | Input Example | Response |
|---|------------|---------------|----------|
| 1 | **Blank name** | *(empty)* | `[Error Test Run] Name cannot be blank. Record rejected.` |
| 2 | **Non-numeric pay** | `Ten Thousand` | `[Error Test Run] Invalid data type. Please enter numbers only. Record rejected.` |
| 3 | **Negative pay** | `-10000` | `[Error Test Run] Basic pay cannot be negative. Record rejected.` |

### 6. How to Compile & Run

**Prerequisites**: A C++ compiler (g++, clang++, or MSVC).

```bash
# Compile
g++ -o Group_5 Group_5.cpp

# Run
./Group_5
```

The system requires a **minimum of 5 staff records** before generating the payroll summary report. Type `exit` as the Staff ID when finished entering records.

### 7. Sample Output

#### Error Handling Tests

![Error handling test output showing blank name, invalid data type, and negative value rejection](./screenshots/test_errors.png)

#### Valid Payslip Run (5 Records + Summary Report)

![Complete payroll run showing 5 payslips with shadowing resolution experiments and final summary report](./screenshots/valid_tests.png)

### 8. Conclusion

This project successfully demonstrates the core programming language concepts of **names**, **bindings**, **scopes**, **aliasing**, **shadowing**, and **type inference** within a practical payroll processing context. Each concept is clearly labelled in the source code with comments explaining the underlying theory. The three error handling test cases validate the system’s robustness, while the shadowing experiment and static variable counter provide visible, measurable proof of how the C++ compiler resolves naming conflicts and manages variable lifetimes.

---

<p align="center"><sub>© 2026 Group 5 — CCS 2105 Programming Languages. University of Kabianga.</sub></p>
