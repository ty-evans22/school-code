/*
Tyler Evans
Feb 8, 2023
Activity06 - Variables
*/

// Q1 : Is it permited the next ?
console.log("Q1 ---------------")
var var1 = "Iowa";
console.log(var1);
var var1 = 124;
console.log(var1);
// Is it permited ?
console.log("Yes or Not. Yes");

// Q2 : Is it valid ?
console.log("Q2 ----------------");
let var2 = "Ames";
console.log(var2);
let var2 = 124;
// Is it valid ?
console.log("Yes or Not. No");

// Q3 : Is it valid ?
console.log("Q3 ----------------");
let var3 = "ISU";
console.log(var3);
var3 = 2023;
console.log(var3);
console.log("Valid ? Yes")

// Q4 : Explain the Error.
console.log("Q4 ----------------");
let var4;
const var5;
console.log("What's the error :")

// Q5 : Explain the Error.
console.log("Q5 ----------------");
const var6 = 3.1415;
var6 = 2.8;
console.log("What's the error : var6 is declared as a const so its value cannot be changed. ");

// Q6 : Explain the Error.
let first name = "Abraham";
console.log(" Variable name for first name needs to be one word. ");
let 2numbers = [1,2];
console.log(" Variable name for 2numbers cannot start with a number. ");
let city-state = "Ames Iowa";
console.log(" Variable name for city-state cannot have a hyphen. ");

// Q7 : What !! ??
let mainCity = "DesMoines";
console.log("This is the Capital :", MainCity);
console.log(" The variable called isn't the same name (first letter capitalized). ");

// Q8 : "let" and "const" scope vs "var" scope
if (5 === 5) {
    var var7 = 100;
    }
    console.log(var7);
    if (5 === 5) {
    let var8 = 100;
    }
    console.log(var8);
    console.log("Var8 doesn't exist outside the scope of the if statement.");