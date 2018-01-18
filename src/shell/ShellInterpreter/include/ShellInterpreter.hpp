/* 
 * File:   ShellInterpreter.hpp
 * Author: Vincent
 *
 * Created on 17 juillet 2016, 16:26
 */

#ifndef SHELLINTERPRETER_HPP
#define	SHELLINTERPRETER_HPP

#include "LuaVirtualClass.hpp"

/**
 * Shell implementation.
 *
 * Reads statements from standard input, evaluates them and prints the result
 * on standard output.
 */
class ShellInterpreter : public LuaVirtualClass {
public:
    /**
     * Creates a new shell.
     *
     * @param[in,out] rootObject Main object of the shell.
     * @param[in] rootName Name of the variable in the shell that will contain rootObject.
     */
    ShellInterpreter(LuaVirtualClass& rootObject, const std::string& rootName);

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
    LuaVirtualClass& getRootObject();

    const std::string& luaClassName() const override;

    void luaPopulateIndex(LuaStateView& luaState) override;
private:
    /** Root object of this shell.*/
    LuaVirtualClass& rootObject;

    /** Name of the global variable containing rootObject. */
    std::string rootName;

    /** Flag telling if the main loop should keep running. */
    bool running;
};

#endif	/* SHELLINTERPRETER_HPP */

