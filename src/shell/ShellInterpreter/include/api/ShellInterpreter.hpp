/* 
 * File:   ShellInterpreter.hpp
 * Author: Vincent
 *
 * Created on 17 juillet 2016, 16:26
 */

#ifndef SHELLINTERPRETER_HPP
#define	SHELLINTERPRETER_HPP

#include "ShellClass.hpp"

/**
 * Shell implementation.
 *
 * Reads statements from standard input, evaluates them and prints the result
 * on standard output.
 */
class ShellInterpreter : public ShellClass {
public:
    /**
     * Creates a new shell.
     *
     * the rootObject is the object in which fields and methods will be researched first.
     *
     * @param[in,out] rootObject Main object of the shell.
     */
    ShellInterpreter(ShellClass& rootObject);
    
    virtual const std::string& getShellClassName() const override;

    /**
     * Run the shell.
     *
     * It contains the main loop reading from standard input.
     */
    void run();

    /**
     * Set a flag making the shell to stop after evaluating the current statement.
     */
    void quit();

    /**
     * Get the root object of this shell.
     *
     * @return The root object of this shell.
     */
    ShellClass& getRootObject();
private:
    /** Root object of this shell.*/
    ShellClass& rootObject;

    /** Flag telling if the main loop should keep running. */
    bool running;
};

#endif	/* SHELLINTERPRETER_HPP */

