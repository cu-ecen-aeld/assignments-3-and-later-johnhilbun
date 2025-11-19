#include "systemcalls.h"
#include "stdlib.h"		// JDH for system()
#include <sys/types.h>		// JDH man fork includes this
#include <unistd.h>		// JDH man fork includes this
#include <sys/types.h>		// JDH man waitp includes this
#include <sys/wait.h>		// JDH man waipt includes this
#include <fcntl.h>		// JDH for file redirection 


/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    int result = 0;

    result = system(cmd);
    if (cmd == NULL)
    {
        // printf("cmd is NULL, so I think return true, but is that correct?");
	return true;
    }
    else if (result == -1) 
    {
        // printf("system() failed : returned %d", result);
        return false;
    }
    else if (result == 2)
    {
        // printf("system() could not execute shell. How to see if status is 127?");
	// call wait-pid(2)?
	return false;
    }
    else
    {
        // printf("system() succeeded. termination of child shell is ___?");
	// call wait-pid(2)?
	return true;
    }

    return true;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
    // from Linux System Programming page 161
    {
        int status;
	pid_t pid;

	//for (int i=0; i<count; i++)
	//    printf("\n### do_exec() command: %s###\n",command[i]);

	pid = fork();
	if (pid == -1)
	{
	    //printf("###Parent### do_exec() fork() was -1 : will return false ###\n");
	    return false;
	}
	else if (pid == 0)	// this is the child
	{
	    //printf("###Child### do_exec() will run %s", command[0]);

	    execv (command[0], command); // should not return if it worked?

	    //printf("###Child### do_exec() execv() failed : return false###\n");
	    exit(-1); // execv() failed, clean up the fork()
	}

	if (waitpid (pid, &status, 0) == -1)
	{
	    //printf("###Parent### do_exec() will return false ###\n");
	    return false;
	}

	// Check if child exited successfully
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
	{
	    //printf("###Parent### do_exec() will return true ###\n");
	    return true;
	}
	else
	{
	    //printf("###Parent### do_exec() will return false ###\n");
	    return false;
	}
	
    }

    va_end(args);

    return true;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    va_list args_copy;
    va_copy(args_copy, args);
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
// JDH begin : from Linux System Programming page 161
    {
        int status;
        pid_t pid;

        //for (int i=0; i<count; i++)
        //    printf("\n### do_exec_redirect() command: %s###\n",command[i]);

	// next two lines are for redirection
	int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
	if (fd < 0) {perror("open"); abort(); }

        pid = fork();
        if (pid == -1)
        {
            //printf("###Parent### do_exec_redirect() fork() was -1 : will return false ###\n");
            return false;
        }
        else if (pid == 0)      // this is the child
        {
	    // JDH redirection - begin
	    if (dup2(fd, 1) < 0) { perror("fork()"); abort();}
	    close(fd);
	    // JDH redirection - end
            //printf("###Child### do_exec_redirect() will run %s", command[0]);

            execv (command[0], command); // should not return if it worked?

            //printf("###Child### do_exec_redirect() execv() failed : return false###\n");
            exit(-1); // execv() failed, clean up the fork()
        }
	close(fd);// JDH redirection

	if (waitpid (pid, &status, 0) == -1)
        {
            //printf("###Parent### do_exec_redirect() will return false ###\n");
            return false;
        }

        // Check if child exited successfully
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            //printf("###Parent### do_exec_redirect() will return true ###\n");
            return true;
        }
        else
        {
            //printf("###Parent### do_exec_redirect() will return false ###\n");
            return false;
        }

    }

    va_end(args);

    // JDH original : return true;
}
