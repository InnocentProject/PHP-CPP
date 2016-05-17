/**
 *  ConstantFuncs.cpp
 *
 *  C++ implementation of PHP functions to retrieve and set constants.
 *
 *  @author Emiel Bruijntjes <emiel.bruijntjes@copernica.com>
 *  @copyright 2014 Copernica BV
 */

/**
 *  Dependencies
 */
#include "includes.h"

/**
 *  Set up namespace
 */
namespace Php {

/**
 *  Retrieve the value of a constant by its name
 *  @param  name            Name of the constant
 *  @return Value           Actual constant value
 */
Value constant(const char *name)
{
    // pass on to other implementation
    return constant(name, ::strlen(name));
}

/**
 *  Retrieve a constant by its name, and the size of the name
 *  @param  constant        Name of the constant
 *  @param  size            Size of the name
 *  @return Value
 */
Value constant(const char *constant, size_t size)
{
    // we need the tsrm_ls variable
    TSRMLS_FETCH();

    // retrieve the constant
    auto *result = zend_get_constant(zend_string_init(constant, size, 1) TSRMLS_CC);

    // did the constant exist?
    if (!result) return nullptr;

    // return the valid result
    return result;
}

/**
 *  Retrieve the value of a constant by its name
 *  @param  name            Name of the constant
 *  @return Value           Actual constant value
 */
Value constant(const std::string &name)
{
    // pass on to other implementation
    return constant(name.c_str(), name.size());
}

/**
 *  Define a new constant
 *  @param  name            Name of the constant
 *  @param  size            Size of the name
 *  @param  value           Value of the constant
 *  @return bool
 */
bool define(const char *name, size_t size, const Value &value)
{
    // we need the tsrm_ls variable
    TSRMLS_FETCH();

    // the constant structure from the zend engine
    zend_constant constant;

    // copy the name
    constant.name = zend_string_init(name, size, 1);

    // only scalar values can be used for constants
    if (value.isScalar())
    {
        // make a full copy of the passed in zval
        constant.value = *value._val;
        zval_copy_ctor(&constant.value);
    }
    else
    {
        // we're going to convert the value object into a string, and use that
        Value str = value.clone(Type::String);

        // use the copied value
        constant.value = *str._val;
        zval_copy_ctor(&constant.value);
    }

    // constants are case sensitive (but not persistent, because this is a user
    // space constant!)
    constant.flags = CONST_CS;

    // as module number we use a fake module number
    constant.module_number = PHP_USER_CONSTANT;

    // register the constant
    return zend_register_constant(&constant TSRMLS_CC) == SUCCESS;
}

/**
 *  Define a new constant
 *  @param  name
 *  @param  value
 *  @return bool
 */
bool define(const char *name, const Value &value)
{
    // define a constant
    return define(name, ::strlen(name), value);
}

/**
 *  Define a new constant
 *  @param  name
 *  @param  value
 *  @return bool
 */
bool define(const std::string &name, const Value &value)
{
    // pass on to the other define function
    return define(name.c_str(), name.size(), value);
}

/**
 *  Check whether a constant exists
 *  @param  name
 *  @param  size
 *  @return bool
 */
bool defined(const char *name, size_t size)
{
    // we need the tsrm_ls variable
    TSRMLS_FETCH();

    // retrieve the constant
    auto *value = zend_get_constant_ex(zend_string_init(name, size, 1), nullptr, ZEND_FETCH_CLASS_SILENT TSRMLS_CC);

    // check if the value was found
    if (!value) return false;

    // constant exists, but the returned zval should first be destructed
    // @todo: is this necessary in PHP 7?
    zval_dtor(value);

    // done
    return true;
}

/**
 *  Check whether a constant exists
 *  @param  name
 *  @return bool
 */
bool defined(const char *name)
{
    // pass on
    return defined(name, ::strlen(name));
}

/**
 *  Check whether a constant exists
 *  @param  name
 *  @return bool
 */
bool defined(const std::string &name)
{
    // pass on
    return defined(name.c_str(), name.size());
}

/**
 *  End namespace
 */
}

