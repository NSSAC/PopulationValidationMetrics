#! /usr/bin/tcsh

set state = $1
set abbrev = $2

set dir = /project/biocomplexity/nssac/EpiHiperTesting/${state}_2017_SynPop
set targetDir = /project/biocomplexity/nssac/SyntheticPopulations/usa_840/2020/v_1_0/v-and-v/contacts-by-age
set exec = ~/PopulationValidationMetrics/AgeGroupMixingMatrix/Contacts 
set popName = ${abbrev}_persontrait_epihiper

set cfg = ${state}.cfg
echo "Output file = ${targetDir}/${state}_athome/${popName}-hh-cm" > $cfg
echo "Population File =  $dir/${popName}.txt" >> $cfg
echo "Age Groups = CDC" >> $cfg
# echo "Age Groups = PolyMod" >> $cfg

$exec $cfg
