/* Copyright 2018 Axel Huebl
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
#include <pybind11/pybind11.h>

#include "openPMD/backend/BaseRecord.hpp"
#include "openPMD/backend/Container.hpp"
#include "openPMD/backend/MeshRecordComponent.hpp"

#include <string>

namespace py = pybind11;
using namespace openPMD;


void init_BaseRecord(py::module &m) {
    py::class_<BaseRecord< MeshRecordComponent >, Container< MeshRecordComponent > >(m, "Mesh_Base_Record");

    py::enum_<UnitDimension>(m, "Unit_Dimension")
        .value("L", UnitDimension::L)
        .value("M", UnitDimension::M)
        .value("T", UnitDimension::T)
        .value("I", UnitDimension::I)
        .value("theta", UnitDimension::theta)
        .value("N", UnitDimension::N)
        .value("J", UnitDimension::J)
    ;
}
