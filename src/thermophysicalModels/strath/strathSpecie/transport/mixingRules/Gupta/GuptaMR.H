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

Class
    Foam::GuptaMR

Description
    GuptaMR model.
  
SourceFiles
    GuptaMR.C

\*---------------------------------------------------------------------------*/

#ifndef GuptaMR_H
#define GuptaMR_H

#include "mixingRule.H"

#include "PtrList.H"
#include "FixedList.H"

#include "fundamentalConstants.H"
#include "physicoChemicalConstants.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
  
/*---------------------------------------------------------------------------*\
                             Class GuptaMR Declaration
\*---------------------------------------------------------------------------*/

template<class ThermoType>
class GuptaMR
:
    public mixingRule
{  

protected:
  
    // Protected data
    
        //- Avogadro's number
        static const scalar NA;
        
        //- Boltzmann's constant in SI units
        static const scalar kB;
        
        //- Universal gas constant [J/(mol K)]
        static const scalar Ru;
        
        //- Mathematical constant Pi
        static const scalar pi;
        
        //- Fundamental electric charge in CGS units
        static const scalar eCGS;
        
        
        //- Thermodynamic data of the species
        const PtrList<ThermoType>& speciesThermo_;
        
        //- Dictionary to calculate the collision integrals (transport properties)
        const IOdictionary dict_;
        
        // Collision intergrals
        FixedList<PtrList<PtrList<FixedList<scalar, 4> > >, 2> piOmegaNeutral_;
        FixedList<FixedList<scalar, 3>, 2> piOmegaNonNeutral_;
        

    // Protected member functions
    
        Foam::string numberToString (int number);
        
        //- Collision terms
        inline scalar collisionIntegralNeutral1(label i, label j, const scalar T)
        {
            if(T != 0.0)
            {
                return 0.0; // NOT AVAILABLE IN BETA RELEASE
            }
            else
            {
                return 0.0;
            }
        }
        
        inline scalar collisionIntegralNeutral2(label i, label j, const scalar T)
        {
            if(T != 0.0)
            {
                return 0; // NOT AVAILABLE IN BETA RELEASE
            }
            else
            {
                return 0.0;
            }
        }
        
        void piOmegaNeutralInit(label no, label i, label j);
        
        scalar piOmegaNeutral(label no, label i, label j, const scalar T)
        {
            return 0; // NOT AVAILABLE IN BETA RELEASE
        }
        
        inline scalar collisionIntegralNonNeutral1(label i, label j, const scalar T, const scalar nDe)
        {
            if(T != 0.0)
            {
                return 0; // NOT AVAILABLE IN BETA RELEASE
            }
            else
            {
                return 0.0;
            }
        }
        
        inline scalar collisionIntegralNonNeutral2(label i, label j, const scalar T, const scalar nDe)
        {
            if(T != 0.0)
            {
                return 0; // NOT AVAILABLE IN BETA RELEASE
            }
            else
            {
                return 0.0;
            }
        }
        
        void piOmegaNonNeutralsInit(const word& attractionType, label no);
        
        scalar piOmegaNonNeutrals(label no, const scalar T, const scalar nDe)
        {
            scalar kBCGS(kB*1.0e7); // SI in Joule, CGS in erg
            scalar nDeCGS(nDe*1.0e6); // SI in m^-3, CGS in cm^-3
            scalar DebyeLength = sqrt(kBCGS*T/(4.0*pi*nDeCGS*pow(eCGS, 2.0)));
            scalar Tstar = DebyeLength/pow(eCGS, 2.0)*kBCGS*T;
            
            return 0; // NOT AVAILABLE IN BETA RELEASE
        }
        

public:

    //- Runtime type information
    TypeName("GuptaMR");

    
    // Constructors

        //- Construct from components
        GuptaMR
        (
            rho2ReactionThermo& thermo,
            const compressible::turbulenceModel2& turbulence
        );


      //- Destructor
      ~GuptaMR()
      {}


    // Member Functions

        //- Molar weight for a specie given by index. Defined here
        //  becouse depends on thermoType template
        inline virtual scalar W(label i) const
        {
            return 1.0e-3*speciesThermo_[i].W();
        }
        
        //- Molar concentration of each species
        scalar ms(label i) const
        {
            return W(i)/NA;
        }
        
        //- Coefficient for the calculation of kappat
        scalar asr(label i, label j) const
        {
            return 1.0 + (1.0-ms(i)/ms(j))*(0.45-2.54*ms(i)/ms(j))/pow(1.0+ms(i)/ms(j), 2.0);
        }
        
        //- Viscosity for a specie given by index. Defined here
        //  becouse depends on thermoType template
        inline virtual scalar mu(label i, scalar p, scalar Tt) const
        {
            return speciesThermo_[i].mu(p, Tt);
        }
        
        //- Thermal conductivity, trans-rotational mode, 
        //  for a specie given by index.
        inline virtual scalar kappatr(label i, scalar p, scalar Tt) const
        {
            return speciesThermo_[i].kappatr(p, Tt);
        }
        
        //- Thermal conductivity, vib-electronic mode, 
        //  for a specie given by index.
        inline virtual scalar kappave(label i, scalar p, scalar Tt, scalar Tve) const
        {
            return speciesThermo_[i].kappave(p, Tt, Tve);
        }
        
        //- Thermal diffusivity [kg/ms], trans-rotational mode, 
        //  for a specie given by index.
        virtual inline scalar alphatr(label i, scalar p, scalar Tt) const
        {
            return speciesThermo_[i].alphatr(p, Tt);
        }
        
        //- Thermal diffusivity [kg/ms], vib-electronic mode, 
        //  for a specie given by index.
        virtual inline scalar alphave(label i, scalar p, scalar Tt, scalar Tve) const
        {
            return speciesThermo_[i].alphave(p, Tt, Tve);
        }
        
        //- Heat capacity at constant pressure [J/(kg K)], trans-rotational mode, 
        //  for a specie given by index.
        virtual inline scalar Cv_t(label i, scalar p, scalar Tt) const
        {
            return speciesThermo_[i].Cv_t(p, Tt);
        }
        
        //- Heat capacity at constant volume [J/(kg K)], vib-electronic mode, 
        //  for a specie given by index.
        virtual inline scalar Cv_vel(label i, scalar p, scalar Tve) const
        {
            return speciesThermo_[i].Cv_vel(p, Tve);
        }
        
        // Calculations and updates of the different fields
        virtual void correct();
        
        //- Write the user-defined GuptaMR quantities
        virtual void write();
                
        //- Read transportProperties dictionary
        virtual bool read();
  };

  
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

}	// End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifdef NoRepository
#   include "GuptaMR.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
