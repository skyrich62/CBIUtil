# CBIUtil
CBI Utilities.  This is a set of useful C++17 programming tools.  Extensive documentation is
available in the source header files in Doxygen format.

# Table of contents

   * CheckPoint
   * TaskQueue
   * ThreadPool
  
# CheckPoint
These are a set of useful programming utilities to help debug and or identify
parts of code in a Debug build but are completely removed in a Release build.
Use the environment variable **CBI_CHECKPOINTS** to control the behavior of 
checkpoints at run-time.

### CBI_CHECKPOINTS environment variable
The **CBI_CHECKPOINTS** variable is a list of categories and/or directives separated by
colons. There are 5 different directives

#### CBI_CHECKPOINTS directives

   * "all" --This turns on all *debug CheckPoints*.
   * "\*" -- Synonym for "all"
   * "except-off" -- Turns off all *exception CheckPoints*.
   * "except-fatal" -- Causes all *exception CheckPoints* to call abort() when activated.
   * "except-crash" -- Causes all "*exception CheckPoints*" to cause a fatal crash when activated.
  
#### CBI_CHECKPOINTS categories
Debug categories are user-defined strings, (but must not match any of the above directives).  The
categories are assinged to a particular instance of a CheckPoint object at contruction.  The CheckPoint
is the activated by ensuring that it's category is in the CBI_CHECPOINTS list, either explicitly or 
implicitly.

#### Example
   ```
   CBI_CHECKPOINTS="foo:bar:expect-fatal" /path/to/some/app {command-line}
   ```
 This will enable user-defined categories "foo" and "bar", as well as cause all *exception CheckPoints*
 to be fatal via calling abort() if activated.

## Kinds of CheckPoints
CheckPoints are used to print out debugging or exception information.  There are two
kinds of CheckPoints: *debug* and *exception*.

### Debug Checkpoints
Debug Checkpoints require a CheckPoint object and are controlled by calling non-static
member functions.  They are constructed with a user-defined category as follows:

   ```
   CheckPoint dbg("foo");
   ```
This creates a *debug CheckPoint* with category "foo".  Once a checkpoint has been constructed,
it can be used to print messages, or perform some debug-only function:

   ```
   dbg.print(CBI_HERE, "The value of _state is: ", _state, ", and _quantity is: ", _quantity);
   ```

This will only print the given message if the the CheckPoint is enabled via **CBI_CHECKPOINTS** and
the build is a debug, rather than a release build.

Additionally, a *debug CheckPoint* can be queried at run-time to see if it's enabled:

   ```
   if (dbg.active()) {
       doSomeDebuggingThing();
   }
   ```
   
This is particularly helpful in a server app to force the server to wait until a debugger is attached.
Put the following code early in the main() function:

   ```
   CheckPoint init("hang-server");
   if (init.active()) {
       bool hang = true;
       while (hang) 
           ;
   }
   ```
Now, let's suppose that the name of the server app is "server".  Start it as follows:

   ```
   CBI_CHECKPOINTS="hang-server" server {command-line args} &
   ```

When the program gets to the while-loop above it will spin endlessly, given the developer plenty
of time to attach a debugger.  Once that has happened, simply set "hang" to "false", set some interesting
breakpoints, and let the server continue running.

### Exception CheckPoints
**Exception CheckPoints** use static member functions and do not have categories associated with them.
There are 2 kinds of **Exception CheckPoints**

   * Qualified -- This CheckPoint only "fires" if a certain condition is met.
   * Unqualified -- This Checkpoint always "fires"

#### Qualified CheckPoints
These are used to manage programatic expectations.  For example, function takes a pointer as an
argument, but doesn't expect that the pointer should be null:

   ```
   void foo(char *p)
   {
       CheckPoint::expect(CBI_HERE, p != nullptr, "p must not be null!");
       . . .
   }
   ```
This CheckPoint will "fire" if p is in fact null.  If so, the message will be printed, and
depending on the contents of the CBI_CHECKPOINTS variable, either the program will continue,
or it will abort() or crash.

#### Unqualified CheckPoints
These are helpful to mark certain places in the code where execution is not expected, (for
example in the "default" clause of a switch statement:

   ```
   switch (c) {
   case 1:
       doSomething();
       break;
   case 2:
       doSomethingElse();
       break;
   default:
       CheckPoint::hit(CBI_HERE, "Shouldn't have gotten here, c = ", c);
       break;
   }
   ```
## CB_HERE
You've probably noticed the CBI_HERE argument used in these CheckPoint calls.  This is simply
a convenient mechanism for describing the current source code location.

## Examples of use
The test.cpp file contains some examples of using CheckPoints.  Here is an example of the output:

   ```
   @@@ CheckPoint (Exception): src/test.cpp:62 (main)
   @@@ test hit

   ```
This was a CheckPoint::hit().  It was on test.cpp, line 22 in the main() function.  It was
coded as follows:

   ```
   cbi::CheckPoint::hit(CBI_HERE, "test hit\n");
   ```
   
# TaskQueue
A TaskQueue holds a list of std::function, (Task), objects and allows clients to add new
tasks and take tasks away.

# ThreadPool
A ThreadPool holds a TaskQueue and starts a number of threads each of which waits for the queue
to be non-empty, whereupon, each thread takes a single task from the queue, and runs it.  Once it
finishes, the thread resumes waiting for a new task.
