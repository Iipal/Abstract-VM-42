# Abstract-VM-42
###### made by tmaluh __(\_ipal)__

## Compiling

Use [make](https://en.wikipedia.org/wiki/Makefile) for compiling all files.

When you have already installed SDL2 library you can use this simple rules:
- **make**: Compile all.
- **make clean**: Delete all AVM temporary files.
- **make del**: Equals to **make clean**.
- **make fclean**: Expands **make clean** to delete also executable AVM.
- **make re**: It's rules - **make fclean** & **make** in one.
- **make pre**: It's rules - **make del** & **make** in one.
- **make debug_all**: Re-compile AVM without optimization flags but with `-g3 -fsanitize=address` flags for debug.
- **make debug**: Compile AVM without optimization flags but with `-g3 -fsanitize=address` flags for debug.

## Usage:

> File input mode:
```bash
$> ./avm <file-name> ... <N-file-name>
```
> File input mode with multi file stack mode:
```bash
$> ./avm -mfs <file-name> ... <N-file-name>
```
> >  `-mfs` flag is: read commands from all files to a single commands stack and execute. Useless for only 1 file.

> Pipe input mode:
```bash
$> cat <file-name> | ./avm
```

> Standard input mode:
```bash
$> ./avm
```

## Documentation:

|  command  | parameter     | description                                                                                 |
| --------- | ------------- | ------------------------------------------------------------------------------------------- |
| exit      |               | Stop to execute command queue and exit from AVM (necessary at the end of command queue)     |
| print     |               | Asserts that the value at the top of the stack is an 8-bit integer                          |
| assert    | @exception    | check if @exception is true or not                                                          |
| push      | @type(@value) | valid @type is int8, int16, int32, float, double; Pushes the @value at the top of the stack |
| pop       |               | Unstacks the value from the top of the stack                                                |
| add       |               | Unstacks the first two values on the stack, adds them, then stacks the result               |
| sub       |               | Unstacks the first two values on the stack, subtracts them, then stacks the result          |
| mul       |               | Unstacks the first two values on the stack, multiplies them, then stacks the result         |
| div       |               | Unstacks the first two values on the stack, divides them, then stacks the result            |
| mod       |               | Unstacks the first two values on the stack, calculates the modulus, then stacks the result  |
|           |               | Empty lines will be ignored                                                                 |

> Specified, for standard input mode, commands:

|  command  | parameter     | description                                              |
| --------- | ------------- | -------------------------------------------------------- |
| ;;        |               | Stops waiting for any input and execute AVM              |
| clean/c   |               | Remove all commands from queue                           |
| delete/d  |               | Remove last added command from queue                     |
| quit/q    |               | Stops waiting for any input and quit without execute AVM |
| list/l    |               | Show list of currently added commands to queue           |
| help/h    |               | Print this help info                                     |
