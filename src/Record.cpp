/* Copyright 2017-2018 Fabian Koller
 *
 * This file is part of openPMD-api.
 *
 * openPMD-api is free software: you can redistribute it and/or modify
 * it under the terms of of either the GNU General Public License or
 * the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * openPMD-api is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License and the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * and the GNU Lesser General Public License along with openPMD-api.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#include "openPMD/Record.hpp"

#include <iostream>


namespace openPMD
{
Record::Record()
{
    setTimeOffset(0.f);
}

Record::Record(Record const&) = default;
Record::~Record() = default;

Record&
Record::setUnitDimension(std::map< UnitDimension, double > const& udim)
{
    if( !udim.empty() )
    {
        std::array< double, 7 > tmpUnitDimension = this->unitDimension();
        for( auto const& entry : udim )
            tmpUnitDimension[static_cast<uint8_t>(entry.first)] = entry.second;
        setAttribute("unitDimension", tmpUnitDimension);
    }
    return *this;
}

void
Record::flush(std::string const& name)
{
    if( !written )
    {
        if( *m_containsScalar )
        {
            RecordComponent& r = at(RecordComponent::SCALAR);
            r.m_writable->parent = parent;
            r.parent = parent;
            r.flush(name);
            m_writable->abstractFilePosition = r.m_writable->abstractFilePosition;
            abstractFilePosition = r.abstractFilePosition;
            written = true;
        } else
        {
            Parameter< Operation::CREATE_PATH > pCreate;
            pCreate.path = name;
            IOHandler->enqueue(IOTask(this, pCreate));
            IOHandler->flush();
            for( auto& comp : *this )
                comp.second.parent = getWritable(this);
        }
    }

    for( auto& comp : *this )
        comp.second.flush(comp.first);

    flushAttributes();
}

void
Record::read()
{
    /* allow all attributes to be set */
    written = false;

    if( *m_containsScalar )
    {
        /* using operator[] will incorrectly update parent */
        this->at(RecordComponent::SCALAR).read();
    } else
    {
        clear_unchecked();
        Parameter< Operation::LIST_PATHS > pList;
        IOHandler->enqueue(IOTask(this, pList));
        IOHandler->flush();

        Parameter< Operation::OPEN_PATH > pOpen;
        for( auto const& component : *pList.paths )
        {
            RecordComponent& rc = (*this)[component];
            pOpen.path = component;
            IOHandler->enqueue(IOTask(&rc, pOpen));
            IOHandler->flush();
            *rc.m_isConstant = true;
            rc.read();
        }

        Parameter< Operation::LIST_DATASETS > dList;
        IOHandler->enqueue(IOTask(this, dList));
        IOHandler->flush();

        Parameter< Operation::OPEN_DATASET > dOpen;
        for( auto const& component : *dList.datasets )
        {
            RecordComponent& rc = (*this)[component];
            dOpen.name = component;
            IOHandler->enqueue(IOTask(&rc, dOpen));
            IOHandler->flush();
            rc.written = false;
            rc.resetDataset(Dataset(*dOpen.dtype, *dOpen.extent));
            rc.written = true;
            rc.read();
        }
    }

    readBase();

    readAttributes();

    /* this file need not be flushed */
    written = true;
}
} // openPMD
