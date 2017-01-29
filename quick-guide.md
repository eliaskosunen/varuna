# Primitive types

No implicit conversions between any of the types are allowed.

## Integer types

| Typename | Size (bits) | Literal suffix |
| -------- | ----------- | -------------- |
| `int`    | 32          | (none)         |
| `int8`   | 8           | `i8`           |
| `int16`  | 16          | `i16`          |
| `int32`  | 32          | `i32`          |
| `int64`  | 64          | `i64`          |

All integer types are signed.  


## Floating-point types

| Typename | Size (bits) | Literal suffix |
| -------- | ----------- | -------------- |
| `float`  | 32          | (none)         |
| `f32`    | 32          | `f32`          |
| `f64`    | 64          | `f64`          |

## Other types

`void`: Unsized type. Used only for describing functions that return no value.  
`bool`: Boolean type. Allowed values: `true`, `false`.  
`byte`: 8-bit unsigned integer. Literal suffix: `o` (stands for 'octet')  
`char`: 32-bit unsigned character (UTF-8 in source code). Literal syntax: `'a'`

# Comments

```
// C++-style single-line comments
```

# Variables

Variables are immutable by default.

## Variable definitions

```
// Prefer initializing your variables
let a = 1;       // Immutable variable, type int, value 1
let mut b = 1.0; // Mutable variable, type float, value 1.0
var c = true;    // Mutable variable, type bool, value true

// Avoid this
let d: int8;     // Uninitialized immutable variable, type int8
let mut e: f64;  // Uninitialized mutable variable, type f64
var f: char;     // Uninitialized mutable variable, type char

// Don't do this
let g: byte = 0b01010101b; // Immutable variable, type byte, value 85
```

# Operations

## Binary operations

`+`, `-`, `*`, `/`, `%` work as expected. Operands must be of same type and numeric.

`==`, `!=`, `<`, `>`, `<=`, `>=` work as expected. Operands must be of same type.

`and`, `or` work as expected. Operands must be of type `bool`.

## Unary operations

`not` works as expected. Operand must be of type `bool`.

`+` casts to `int`. Operand must be of an integral type.

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
def function(param: int): int;

// Function definition
def function2(): int {
    return 1;
}
```

## Function calling

```
def function(param: int): int;
def function2(): void;

def callee(): bool {
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
    while (true) {

    }
}
```

# Other expressions

## Casts

`cast<type>(value)`.

Invalid casts produce a compile-time error.
