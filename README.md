# Flint (Programming Language)
Welcome to the official documentation of the Flint programming language. Flint is an easy to use programming language with a custom compiler made in C. You can find the usage and syntax of all the keywords available in this language, along with examples.

---

## Table of Contents
1. [Introduction](#1-introduction)
2. [Data Types](#2-data-types)
3. [Variables](#3-variables)
4. [Operators](#4-operators)
5. [Control Flow](#5-control-flow)
6. [Functions](#6-functions)
7. [Modules](#7-modules)
8. [Error Handling](#8-error-handling)
9. [Lambda Expressions](#9-lambda-expressions)
10. [Example Programs](#10-example-programs)

---

## 1. Introduction
This language is designed to be intuitive and expressive, featuring clear syntax for common programming patterns and modern language features like modules, lambda expressions, and error handling.

---

## 2. Data Types
The language supports several basic data types:

- `num`: Numeric values (integers and decimals)
- `bool`: Boolean values (true/false)
- `float`: Floating-point numbers
- `text`: Text strings
- `char`: Single characters
- `func`: Function type for lambda expressions

---

## 3. Variables
Variables are declared using the `let` keyword and initialized using `set`.

### Declaration Syntax:
```
let <variableName> : <type>;
```

### Examples:
```
let x : num;
let isValid : bool;
let message : text;
let grade : float;
let initial : char;
```

### Assignment Syntax:
```
set <variableName> = <value>;
```

### Examples:
```
set x = 42;
set isValid = true;
set message = "Hello, World!";
set grade = 3.14;
set initial = "A";
```

---

## 4. Operators

### Arithmetic Operators:
- `++>`: Addition (e.g., `5 ++> 3` equals 8)
- `-->`: Subtraction (e.g., `5 --> 3` equals 2)
- `*>`: Multiplication (e.g., `5 *> 3` equals 15)
- `/>`: Division (e.g., `6 /> 2` equals 3)
- `**>`: Exponentiation (e.g., `2 **> 3` equals 8)
- `//>`: Integer Division (e.g., `5 //> 2` equals 2)
- `%>`: Modulo (e.g., `5 %> 2` equals 1)

### Logical Operators:
- `&&`: Logical AND
- `||`: Logical OR
- `!`: Logical NOT
- `==`: Equal to
- `!=`: Not equal to
- `>`: Greater than
- `<`: Less than

### Ternary Operator:
```
set result = <condition> ? <trueValue> : <falseValue>;
```

---

## 5. Control Flow

### Conditional Statements:
```
check <condition> {
    // code
} othercheck <condition> {
    // code
} other {
    // code
}
```

### Loops:
Infinite loop:
```
loop {
    // code
    stop;  // to break the loop
}
```

Range-based loop:
```
loop range 1 -> 10 {
    // code
}
```

---

## 6. Functions

### Function Declaration:
```
funcdo functionName(<parameters>) : <returnType>
    // function body
done
```

### Default Parameters:
```
funcdo greet(name = "world") : text
    echo "Hello" ++> name;
done
```

### Function Call:
```
functionName(arguments);
```

---

## 7. Modules
Modules allow you to organize related functions into a single namespace.

### Module Definition:
```
module moduleName {
    funcdo function1(params) : returnType
        // function body
    done

    funcdo function2(params) : returnType
        // function body
    done
}
```

### Module Usage:
```
echo moduleName.function1(arguments);
```

---

## 8. Error Handling
Error handling is accomplished using run-catch blocks:

```
run {
    // code that might cause an error
} catch {
    // error handling code
}
```

---

## 9. Lambda Expressions
Lambda expressions provide a way to create anonymous functions:

```
let operation : func;
set operation = (a, b) => a ++> b;
```

---

## 10. Example Programs

### Basic Calculator:
```
module calculator {
    funcdo add(a, b) : num
        return a ++> b;
    done
    
    funcdo multiply(a, b) : num
        return a *> b;
    done
}

let result : num;
set result = calculator.add(5, 3);
echo result;
```

### Temperature Converter:
```
funcdo celsiusToFahrenheit(celsius = 0) : float
    return (celsius *> 9 /> 5) ++> 32;
done

let temp : float;
set temp = celsiusToFahrenheit(25);
echo "Temperature in Fahrenheit:";
echo temp;
```

---

## Comments

### Single-line comments:
```
// This is a single-line comment
```

### Multi-line comments:
```
/* This is a
   multi-line comment */
```