/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011 OpenFOAM Foundation
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
    Foam::radiation::spSootRadFracEmission

Description
    spSootRadFracEmission radiation absorption and emission coefficients
    for continuous phase

    The coefficients for the species in the Look up table have to be specified
    for use in moles x P [atm], i.e. (k[i] = species[i]*p*9.869231e-6).

    The coefficients for CO and soot or any other added are multiplied by the
    respective mass fraction being solved

    All the species in the dictionary need either to be in the look-up table or
    being solved. Conversely, all the species solved do not need to be included
    in the calculation of the absorption coefficient

    The names of the species in the absorption dictionary must match exactly the
    name in the look-up table or the name of the field being solved

    The look-up table ("speciesTable") file should be in constant

    i.e. dictionary
    \verbatim
        LookUpTableFileName     "speciesTable";

        EhrrCoeff       0.0;

        CO2
        {
            Tcommon     300.;   // Common Temp
            invTemp     true;   // Is the polynomial using inverse temperature?
            Tlow        300.;   // Low Temp
            Thigh       2500.;  // High Temp

            loTcoeffs           // coeffs for T < Tcommon
            (
                0               //  a0            +
                0               //  a1*T          +
                0               //  a2*T^(+/-)2   +
                0               //  a3*T^(+/-)3   +
                0               //  a4*T^(+/-)4   +
                0               //  a5*T^(+/-)5   +
            );
            hiTcoeffs           // coeffs for T > Tcommon
            (
                18.741
                -121.31e3
                273.5e6
                -194.05e9
                56.31e12
                -5.8169e15
            );
        }
    \endverbatim

SourceFiles
    spSootRadFracEmission.C

\*---------------------------------------------------------------------------*/

#ifndef spSootRadFracEmission_H
#define spSootRadFracEmission_H

#include "interpolationLookUpTable.H"
#include "absorptionEmissionModel.H"
#include "HashTable.H"
#include "absorptionCoeffs.H"
#include "basicThermo.H"

#include "LESModel.H"

#include "psiReactionThermo.H"
#include "thermoPhysicsTypes.H"
#include "reactingMixture.H"
#include "singleStepCombustion.H"

//#include "OpenFOAM.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace radiation
{

/*---------------------------------------------------------------------------*\
                 Class spSootRadFracEmission Declaration
\*---------------------------------------------------------------------------*/

class spSootRadFracEmission
:
    public absorptionEmissionModel
{
public:

    // Public data

        // Maximum number of species considered for absorptivity
        static const int nSpecies_ = 5;

        // Absorption Coefficients
        absorptionCoeffs coeffs_[nSpecies_];
	//


private:

    // Private data

        //- Absorption model dictionary
        dictionary coeffsDict_;

        //- SLG thermo package
        const basicThermo& thermo_;

        //- Turbulence package
        const turbulenceModel& turbulence_;

        //- Emission constant coefficient
        const scalar EhrrCoeff_;

        //- Switch for scaling radiant fraction
        Switch radScaling;

        //- Radiant fraction for fuel from patch 1 and 2
        scalar Ehrr1_;
        scalar Ehrr2_;

        //- patch names for patches 1 and 2
        wordList patchName1_;
        wordList patchName2_;

	//- lookup table related
	scalar YO2Inf;
	scalar Ceta;
	scalar Ceta0;
	word tableName_;
	bool globalYO2_;
	word fuel_;
	scalarList tableOxyMassFracs_;
	//scalar psiR_;

	//- Table values
	List< List< scalar > > srTables_;
	List< List< scalar > > hpTables_;
	List< List< scalar > > psiRTables_;

	// Interpolated values
	List< scalar > sr_;
	List< scalar > hp_;
	//List< scalar > psiR_;

	// epsilon_
	//volScalarField epsilon_;
	// PsiR
	//volScalarField PsiR_;
	// strainRate
	mutable volScalarField strainRate_;
	// priorLoss
	//volScalarField priorLoss_;
	
        inline scalar psiR(const label i, const label j, const label k) const;


public:

    //- Runtime type information
    TypeName("spSootRadFracEmission");


    // Constructors

        //- Construct from components
        spSootRadFracEmission
        (
            const dictionary& dict,
            const fvMesh& mesh
        );


    //- Destructor
    virtual ~spSootRadFracEmission();


    // Member Operators

        // Access

            // Absorption coefficient

                //- Absorption coefficient for continuous phase
                tmp<volScalarField> aCont(const label bandI = 0) const;


            // Emission coefficient

                //- Emission coefficient for continuous phase
                tmp<volScalarField> eCont(const label bandI = 0) const;


            // Emission contribution

                //- Emission contribution for continuous phase
                tmp<volScalarField> ECont(const label bandI = 0) const;


    // Member Functions

        inline bool isGrey() const
        {
            return true;
        }


	//- Table-lookup functions
	void readTableData();
	tmp<volScalarField> computeQr() const;
	//void computeQr() const;
	scalar interpolatePsiR(const label i = 0, const label j = 0, const label k = 0, const scalar sRate = 0, const scalar pLoss = 0) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace radiation
} // End namespace Foam

#include "spSootRadFracEmissionI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
