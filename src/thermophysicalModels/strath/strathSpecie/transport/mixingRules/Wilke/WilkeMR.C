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

\*---------------------------------------------------------------------------*/

#include "WilkeMR.H"
#include "fvm.H"

// * * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * //

template<class ThermoType>
void Foam::WilkeMR<ThermoType>::updatePhi()
{     
    const volScalarField& p = thermo_.p();
    const volScalarField& Tt = thermo_.Tt();
    const scalarField& pCells = p.internalField();
    const scalarField& TtCells = Tt.internalField();
    
    forAll(species(), speciei)
    { 
        volScalarField& phi = phi_[speciei];
        scalarField& phiCells = phi.internalField();
        
        phiCells = 1.0;
        
        forAll(Tt.boundaryField(), patchi)
        {
            phi.boundaryField()[patchi] = 1.0;
        }
        
        forAll(species(), speciej)
        { 
            const volScalarField& Xj = thermo_.composition().X(speciej);
            const scalarField& XjCells = Xj.internalField();
               
            forAll(phiCells, celli)
            {  
                if(miniXs_ < XjCells[celli])
                {
                    phiCells[celli] += XjCells[celli]*sqr
                    (
                        1.0 + sqrt(mu(speciei, pCells[celli], TtCells[celli])/mu(speciej, pCells[celli], TtCells[celli])) 
                      * pow025(W(speciej)/W(speciei))
                    ) 
                      /sqrt(8.0*(1.0+W(speciei)/W(speciej)));
                }
            }
        
            forAll(Tt.boundaryField(), patchi)
            { 
                const fvPatchScalarField& pXj = Xj.boundaryField()[patchi]; 
                const fvPatchScalarField& pp = p.boundaryField()[patchi];
                const fvPatchScalarField& pTt = Tt.boundaryField()[patchi];
                fvPatchScalarField& pphi = phi.boundaryField()[patchi];
                
                forAll(pTt, facei)
                {
                    if(miniXs_ < pXj[facei])
                    {
                        pphi[facei] += pXj[facei]*sqr
                        (
                            1.0 + sqrt(mu(speciei, pp[facei], pTt[facei])/mu(speciej, pp[facei], pTt[facei])) 
                          * pow025(W(speciej)/W(speciei))
                        )
                         /sqrt(8.0*(1.0+W(speciei)/W(speciej)));
                    }
                }   
            }
        }//end secondary species loop
    }//end primary species loop
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class ThermoType>
Foam::WilkeMR<ThermoType>::WilkeMR
(
    rho2ReactionThermo& thermo,
    const compressible::turbulenceModel2& turbulence
)
:
    mixingRule(thermo, turbulence),
    
    speciesThermo_
    (
        dynamic_cast<const multi2ComponentMixture<ThermoType>&>
            (this->thermo_).speciesData()
    ),
    
    miniXs_(1.0e-8)
{     
    phi_.setSize(species().size());
       
    forAll(phi_, speciei)
    {
        phi_.set
        (
            speciei, 
            new volScalarField
            (
                IOobject
                (
                    "phi_" + species()[speciei],
                    mesh_.time().timeName(),
                    mesh_,
                    IOobject::NO_READ,
                    IOobject::NO_WRITE
                ),
                mesh_,
                dimless
            )
        );
    }   
    
    correct();
}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

template<class ThermoType>
void Foam::WilkeMR<ThermoType>::correct()
{
    updatePhi();

    const volScalarField& p = thermo_.p();
    const volScalarField& Tt = thermo_.Tt();
    const scalarField& pCells = p.internalField();
    const scalarField& TtCells = Tt.internalField();
    
    volScalarField& muMix = thermo_.mu();
    volScalarField tempoMu = muMix;
    
    volScalarField& kappaMix = thermo_.kappatr();
    volScalarField& kappaveMix = thermo_.kappave();
    volScalarField tempoKappatr = kappaMix;
    volScalarField tempoKappave = kappaveMix;
    
    volScalarField& alphaMix = thermo_.alphatr();
    volScalarField& alphaveMix = thermo_.alphave();
    volScalarField tempoAlphatr = alphaMix;
    volScalarField tempoAlphave = alphaveMix;
    
    scalarField& muCells = tempoMu.internalField();
    scalarField& kappatrCells = tempoKappatr.internalField();
    scalarField& kappaveCells = tempoKappave.internalField();
    scalarField& alphatrCells = tempoAlphatr.internalField();
    scalarField& alphaveCells = tempoAlphave.internalField();
    
    //- Initialisations
    muCells = 0.0;
    kappatrCells = 0.0;
    kappaveCells = 0.0;
    alphatrCells = 0.0;
    alphaveCells = 0.0;
    
    forAll(tempoMu.boundaryField(), patchi)
    {
        fvPatchScalarField& pmu = tempoMu.boundaryField()[patchi];
        fvPatchScalarField& pkappatr = tempoKappatr.boundaryField()[patchi];
        fvPatchScalarField& pkappave = tempoKappave.boundaryField()[patchi];
        fvPatchScalarField& palphatr = tempoAlphatr.boundaryField()[patchi];
        fvPatchScalarField& palphave = tempoAlphave.boundaryField()[patchi];
        
        pmu = 0.0;
        pkappatr = 0.0;
        pkappave = 0.0;
        palphatr = 0.0;
        palphave = 0.0;
        
    }
    
    //- Cell values
    forAll(species(), speciei)
    { 
        const volScalarField& Tve = thermo_.composition().Tv(speciei);
        const volScalarField& X = thermo_.composition().X(speciei);
        
        const scalarField& TveCells = Tve.internalField();
        const scalarField& XCells = X.internalField();
        const scalarField& phiCells = phi_[speciei].internalField();
        scalarField& spmuCells = spmu_[speciei].internalField();
        scalarField& spkappatrCells = spkappatr_[speciei].internalField();
        scalarField& spkappaveCells = spkappave_[speciei].internalField();
        scalarField& spalphatrCells = spalphatr_[speciei].internalField();
        scalarField& spalphaveCells = spalphave_[speciei].internalField();
        
        forAll(XCells, celli)
        {
            spmuCells[celli] = mu(speciei, pCells[celli], TtCells[celli]);
            muCells[celli] += XCells[celli]*spmuCells[celli]/phiCells[celli];
                
            spkappatrCells[celli] = kappatr(speciei, pCells[celli], TtCells[celli]);
            kappatrCells[celli] += XCells[celli]*spkappatrCells[celli]/phiCells[celli];  
            
            spkappaveCells[celli] = kappave(speciei, pCells[celli], TtCells[celli], TveCells[celli]);
            kappaveCells[celli] += XCells[celli]*spkappaveCells[celli]/phiCells[celli];
            
            spalphatrCells[celli] = alphatr(speciei, pCells[celli], TtCells[celli]);
            alphatrCells[celli] += XCells[celli]*spalphatrCells[celli]/phiCells[celli];  
            
            spalphaveCells[celli] = alphave(speciei, pCells[celli], TtCells[celli], TveCells[celli]);
            alphaveCells[celli] += XCells[celli]*spalphaveCells[celli]/phiCells[celli];
        }        

        //- Patch values
        forAll(X.boundaryField(), patchi)
        {        
            const fvPatchScalarField& pTve = Tve.boundaryField()[patchi];
            const fvPatchScalarField& pX = X.boundaryField()[patchi];
                       
            const fvPatchScalarField& pTt = Tt.boundaryField()[patchi];
            const fvPatchScalarField& pp = p.boundaryField()[patchi];
            const fvPatchScalarField& pphi = phi_[speciei].boundaryField()[patchi];
            
            fvPatchScalarField& pspmu = spmu_[speciei].boundaryField()[patchi];
            fvPatchScalarField& pmu = tempoMu.boundaryField()[patchi];
            fvPatchScalarField& pspkappatr = spkappatr_[speciei].boundaryField()[patchi];
            fvPatchScalarField& pkappatr = tempoKappatr.boundaryField()[patchi];
            fvPatchScalarField& pspkappave = spkappave_[speciei].boundaryField()[patchi];
            fvPatchScalarField& pkappave = tempoKappave.boundaryField()[patchi];
            fvPatchScalarField& pspalphatr = spalphatr_[speciei].boundaryField()[patchi];
            fvPatchScalarField& palphatr = tempoAlphatr.boundaryField()[patchi];
            fvPatchScalarField& pspalphave = spalphave_[speciei].boundaryField()[patchi];
            fvPatchScalarField& palphave = tempoAlphave.boundaryField()[patchi];
            
            forAll(pX, facei)
            { 
                pspmu[facei] = mu(speciei, pp[facei], pTt[facei]);
                pmu[facei] += pX[facei]*pspmu[facei]/pphi[facei];       
                    
                pspkappatr[facei] = kappatr(speciei, pp[facei], pTt[facei]);
                pkappatr[facei] += pX[facei]*pspkappatr[facei]/pphi[facei];   
                    
                pspkappave[facei] = kappave(speciei, pp[facei], pTt[facei], pTve[facei]);
                pkappave[facei] += pX[facei]*pspkappave[facei]/pphi[facei]; 
                
                pspalphatr[facei] = alphatr(speciei, pp[facei], pTt[facei]);
                palphatr[facei] += pX[facei]*pspalphatr[facei]/pphi[facei];   
                    
                pspalphave[facei] = alphave(speciei, pp[facei], pTt[facei], pTve[facei]);
                palphave[facei] += pX[facei]*pspalphave[facei]/pphi[facei]; 
            }  
        }
    }
       
    muMix = tempoMu;
    kappaMix = tempoKappatr;
    kappaveMix = tempoKappave;
    alphaMix = tempoAlphatr;
    alphaveMix = tempoAlphave;
} 


template<class ThermoType>
void Foam::WilkeMR<ThermoType>::write()
{
    if (writeMuSpecies_)
    {
        forAll(species(), speciei)
        {  
            spmu_[speciei].write();   
        }      
    } 
    
    if (writeMuMixture_)
    {
        thermo_.mu().write();     
    }
    
    if (writeKappaSpecies_)
    {
        forAll(species(), speciei)
        {  
            spkappatr_[speciei].write(); 
            spkappave_[speciei].write();  
        }      
    } 
    
    if (writeKappaMixture_)
    {
        thermo_.kappatr().write(); 
        thermo_.kappave().write();    
    }      
} 


template<class ThermoType>
bool Foam::WilkeMR<ThermoType>::read()
{
    if (regIOobject::read())
    {
        return true;
    }
    else
    {
        return false;
    }
}
   

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
