/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2012 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "rhoThermoHTC.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::hTCModels::rhoThermoHTC::rhoThermoHTC
(
    const word& modelType,
    const fvMesh& mesh
)
:
    rhoHTCModel(modelType, mesh),
    thermoPtr_(rhoReactionThermo::New(mesh))
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::hTCModels::rhoThermoHTC::~rhoThermoHTC()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

Foam::rhoReactionThermo&
Foam::hTCModels::rhoThermoHTC::thermo()
{
    return thermoPtr_();
}


const Foam::rhoReactionThermo&
Foam::hTCModels::rhoThermoHTC::thermo() const
{
    return thermoPtr_();
}


Foam::tmp<Foam::volScalarField>
Foam::hTCModels::rhoThermoHTC::rho() const
{
    return thermoPtr_().rho();
}


// ************************************************************************* //
