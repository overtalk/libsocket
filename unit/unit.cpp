/**
 * Created by Jian Chen
 * @since  2015.07.25
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://www.chensoft.com
 */
#include "unit.h"
#include "unit_cmd.h"

using namespace chen;

// -----------------------------------------------------------------------------
// unit
#pragma mark -
#pragma mark unit

unit& unit::instance()
{
    static unit inst;
    return inst;
}

unit::unit()
{
    // register all unit test
    this->_store["cmd"] = new unit_cmd;
}

unit::~unit()
{
    // delete all unit test
    for (auto it = this->_store.begin(); it != this->_store.end(); ++it)
        delete it->second;
}

void unit::check()
{
    for (auto it = this->_store.begin(); it != this->_store.end(); ++it)
        it->second->check();
}

void unit::bench()
{
    for (auto it = this->_store.begin(); it != this->_store.end(); ++it)
        it->second->bench();
}