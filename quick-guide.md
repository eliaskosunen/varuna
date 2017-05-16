# Primitive types

No implicit conversions between any of the types are allowed.

## Integer types

| Typename | Size (bits) | Literal suffix |
| -------- | ----------- | -------------- |
| `i8`     | 8           | `i8`           |
| `i16`    | 16          | `i16`          |
| `i32`    | 32          | `i32` or none  |
| `i64`    | 64          | `i64`          |

All integer types are signed.  

## Floating-point types

| Typename | Size (bits) | Literal suffix |
| -------- | ----------- | -------------- |
| `f32`    | 32          | `f32`          |
| `f64`    | 64          | `f64` none     |

## Other types

`void`: Unsized type. Used only for describing functions that return no value.  
`bool`: Boolean type. Allowed values: `true`, `false`.  
`byte`: 8-bit unsigned integer. Literal suffix: `o` (stands for 'octet')  
`char`: 32-bit unsigned character (UTF-8 in source code). Literal syntax: `'a'`  
`bchar`: 8-bit unsigned character. Literal syntax: `b'a'`

# Comments

```
// C++-style single-line comments
```

```
/*
C-style multi-line comments
/*
That nest properly
*/
*/
```

# Variables

Variables are immutable by default.

## Variable definitions

```
// Prefer initializing your variables
let a = 1;       // Immutable variable, type i32, value 1
let mut b = 1.0; // Mutable variable, type float, value 1.0
var c = true;    // Mutable variable, type bool, value true

// Avoid this
let d: i8;       // Uninitialized immutable variable, type i8
let mut e: f64;  // Uninitialized mutable variable, type f64
var f: char;     // Uninitialized mutable variable, type char

// Don't do this
let g: byte = 0b01010101b; // Immutable variable, type byte, value 85
```

## Global variables

Variables can be declared global.

```
let global = 1;

def main() -> i32 {
    return global;
}
```

# Operations

## Binary operations

`+`, `-`, `*`, `/`, `%` work as expected. Operands must be of same type and numeric.

`==`, `!=`, `<`, `>`, `<=`, `>=` work as expected. Operands must be of same type.

`and`, `or` work as expected. Operands must be of type `bool`.

## Unary operations

`not` works as expected. Operand must be of type `bool`.

`+` casts to `i32`. Operand must be of an integral type.

`-` works as expected. Operand must be of numeric type.

## Assignment operations

Assignments are only allowed to mutable lvalues.

`=`, `+=`, `-=`, `*=`, `/=`, `%=`

## Arbitrary-operand operations

Function call operator: `()`

# Functions

## Function definitions and declarations

```
// Function declaration
def function(param: i32) -> i32;

// Function definition
def function2() -> i32 {
    return 1;
}

// Omitting the return type means void
def function3();
```

## Function calling

```
def function(param: i32) -> i32;
def function2();

def callee() -> bool {
    let ret = function(1);
    function2();
    return true;
}
```

# Control statements

## If-else

```
// Condition must be (or evaluate to) bool
if 1 == 1 {
    // Parenthesis are optional
    // Sometimes not having parens gives an error
    if (false) {

    }
} else if function() {

} else {

}
```

## For

```
// Loop from 0 to 9
// Loop variable init, condition (bool), step
for var i = 0, i < 10, i += 1 {
    // Parenthesis are optional
    // Sometimes not having parens gives an error
    for (var j = 0, j < 2, j += 1) {
        // Infinite loop
        for ,, {

        }
    }
}
```

## While

```
// Condition must be (or evaluate to) bool
while cond() {
    // Parenthesis are optional
    // Sometimes not having parens gives an error
    while (true) {

    }
}
```

# Type aliases

```
use my_int = i32;
```

# Other expressions

## Casts

`42 as i16`

Invalid casts produce a compile-time error.

### Constructor syntax

`i16(42)`

# Modules

## Creating a module

Include a `module`-statement somewhere in the file, preferably in the beginning.

```
module mymodule;

def private() -> i32 {
    return 0;
}

// Only exported functions can be called!
export def func() -> i32 {
    return private();
}
```

## Importing a module

Modules can be imported with a `import`-statement. The compiler must be able to find the corresponding `.vamod` file in order to successfully compile the module.

```
// Only `func` will be visible
import mymodule;

def main() -> i32 {
    //return private(); <- error
    return func();
}
```

# C interoperability

By default, function names are mangled compatible to the platform C++ ABI.
Currently supported ABIs are MSVC (Windows) and Itanium (other systems).
Declare functions as `nomangle` to disable name mangling to allow calling from C code.

```
export nomangle def c_function();
```
