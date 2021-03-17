#!/bin/sh

random_seed=$1
config=$2

if [ -z $random_seed ]
then
    echo "No random seed specified!"
    exit 1
fi

if [ -z $config ]
then
    echo "No ARGoS configuration file specified!"
    exit 1
fi

output_folder=$(date '+%Y_%m_%d_%s')

mkdir "output/$output_folder"
mkdir "experiments/$output_folder"

temp="experiments/$output_folder/external_simulation_${random_seed}.argos"

cp $config "output/$output_folder"

cp $config $temp

sed -i "s/random_seed=[0-9]*/random_seed=$random_seed/" $temp
sed -i "s/output_folder=\"[a-zA-Z0-9]*\"/output_folder=\"$output_folder\"/" $temp

argos3 -c $temp

rm -rf "experiments/$output_folder" # Delete temporary configuration files (no longer needed)