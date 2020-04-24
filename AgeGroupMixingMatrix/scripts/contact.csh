#! /usr/bin/tcsh

set state = $1
set abbrev = $2

set dir = /project/biocomplexity/nssac/EpiHiperTesting/${state}_2017_SynPop
set exec = ~/PopulationValidationMetrics/AgeGroupMixingMatrix/Contacts
# set exec = ~/contacts/ContactMatrix

set minVal = 5
set maxVal = 100
set alphaVal = 400

set popName = ${abbrev}_persontrait_epihiper.txt
set netDir = model_config_min_${minVal}_max_${maxVal}_alpha_${alphaVal}
set netName = ${abbrev}_contact_network_config_min_${minVal}_max_${maxVal}_alpha_${alphaVal}

set targetDir = /project/biocomplexity/nssac/SyntheticPopulations/usa_840/2020/v_1_0/v-and-v/contacts-by-age
mkdir ${targetDir}/${state}_weekend ${targetDir}/${state}_weekday

set cfg = "${state}_cfg.weekend"
echo "Network File = ${dir}/${netDir}/${netName}_5.txt" > $cfg
echo "Output file = ${targetDir}/${state}_weekend/${netName}" >> $cfg

# set cfg = "${state}_cfg.weekday"
# echo "Network File = ${dir}/${netDir}/${netName}_2.txt" > $cfg
# echo "Output file = ${targetDir}/${state}_weekday/${netName}" >> $cfg

echo "Population File =  $dir/$popName" >> $cfg
echo "Age Groups = CDC" >> $cfg
# echo "Age Groups = PolyMod" >> $cfg
$exec $cfg
