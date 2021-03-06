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

Class
    Foam::psiChemistryHTC

Description
    Compressibility-based chemistry model wrapper for hTC models

SourceFiles
    psiChemistryHTC.C

\*---------------------------------------------------------------------------*/

#ifndef psiChemistryHTC_H
#define psiChemistryHTC_H

#include "autoPtr.H"
#include "psiHTCModel.H"
#include "psiChemistryModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace hTCModels
{

/*---------------------------------------------------------------------------*\
                    class psiChemistryHTC Declaration
\*---------------------------------------------------------------------------*/

class psiChemistryHTC
:
    public psiHTCModel
{
    // Private Member Functions

        //- Construct as copy (not implemented)
        psiChemistryHTC(const psiChemistryHTC&);

        //- Disallow default bitwise assignment
        void operator=(const psiChemistryHTC&);


protected:

    // Protected data

        //- Pointer to chemistry model
        autoPtr<psiChemistryModel> chemistryPtr_;


public:

    // Constructors

        //- Construct from components and thermo
        psiChemistryHTC(const word& modelType, const fvMesh& mesh);


    //- Destructor
    virtual ~psiChemistryHTC();


    // Member Functions

        //- Return access to the thermo package
        virtual psiReactionThermo& thermo();

        //- Return const access to the thermo package
        virtual const psiReactionThermo& thermo() const;

        //- Return const access to the density field
        virtual tmp<volScalarField> rho() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace hTCModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
