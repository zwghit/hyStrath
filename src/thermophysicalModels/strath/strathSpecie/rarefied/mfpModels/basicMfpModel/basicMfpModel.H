/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Namespace
    Foam::basicMfpModels

Description
    Namespace for mfp models.

Class
    Foam::basicMfpModel

Description
    An abstract base class for mfp models.

SourceFiles
    basicMfpModel.C

\*---------------------------------------------------------------------------*/

#ifndef basicMfpModel_H
#define basicMfpModel_H

#include "specie.H"
#include "advancedSpecie.H" // NEW VINCENT
#include "IOdictionary.H"
#include "typeInfo.H"
#include "runTimeSelectionTables.H"
#include "scalarField.H"
#include "volFieldsFwd.H"
#include "surfaceFieldsFwd.H"
#include "dimensionedScalar.H"
#include "tmp.H"
#include "autoPtr.H"

#include "fundamentalConstants.H"
#include "physicoChemicalConstants.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                Class basicMfpModel Declaration
\*---------------------------------------------------------------------------*/

class basicMfpModel
{

protected:

    // Protected data
    
        //- Species properties dictionary
        const dictionary& dict_;
        
        //- Thermophysical properties dictionary
        const dictionary& dictThermoPhy_;

        //- Species name
        const word& name_;
        
        //- Label associated with the species' name
        const label& speciesIndex_;

        // Pressure [Pa]
        const volScalarField& p_;

        // Heavy particle temperature [K]
        const volScalarField& Tt_;


    // Private Member Functions

        //- Disallow copy construct
        basicMfpModel(const basicMfpModel&);

        //- Disallow default bitwise assignment
        void operator=(const basicMfpModel&);


public:

    //- Runtime type information
    TypeName("basicMfpModel");


    // Declare run-time constructor selection table
    
      declareRunTimeSelectionTable
      (
          autoPtr,
          basicMfpModel,
          dictionary,
          (
              const word& name,
              const label& speciesIndex,
              const dictionary& dict,
              const dictionary& dictThermoPhy,
              const volScalarField& p,
              const volScalarField& Tt
          ),
          (name, speciesIndex, dict, dictThermoPhy, p, Tt)
      );


    // Selectors

        //- Return a reference to the selected mfp model
        static autoPtr<basicMfpModel> New
        (
            const word& name,
            const label& speciesIndex,
            const dictionary& dict,
            const dictionary& dictThermoPhy,
            const volScalarField& p,
            const volScalarField& Tt
        );


    // Constructors

        //- Construct from components
        basicMfpModel
        (
            const word& name,
            const label& speciesIndex,
            const dictionary& dict,
            const dictionary& dictThermoPhy,
            const volScalarField& p,
            const volScalarField& Tt
        );


    // Destructor

        virtual ~basicMfpModel()
        {}


    // Member Functions
    
        //- Mean free path [m]
        virtual tmp<volScalarField> mfp() const = 0;
         
        //- Mean free path for patch [m]
        virtual tmp<scalarField> mfp
        (
            const label patchi,
            const scalarField& p,
            const scalarField& Tt            
        ) const = 0;
       
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
