#!/bin/sh

runs=$1
experiment=$2
output_folder=$(date '+%Y_%m_%d_%s') #uses the date command on linux to automatically generate a new folder based on the date and time
									 #preventing consecutive runs from overwriring previous versions.

mkdir -p "output/$output_folder"
mkdir "experiments/$output_folder" #creates temporary folder in experiments folder

config="experiments/$2.argos" #copies original argos file into herer then modifies it

cp $config "output/$output_folder" #copies the config file into the temporary folder

for random_seed in $(seq 1 $runs) # runs the argos file for the amount of runs specified
do
	temp="experiments/$output_folder/temp_${random_seed}.argos" # appends the random seed to this so i get a different one each time i run it

	cp $config $temp #copies the name of the config file for the random seed

	# Different sed syntax required for macOS
	if [ "$(uname)" == "Darwin" ]; then
		sed -i '' "s/random_seed=\"[0-9]*\"/random_seed=$random_seed/" $temp
		sed -i '' "s/output_folder=\"[a-zA-Z0-9]*\"/output_folder=\"$output_folder\"/" $temp
	else
		sed -i "s/random_seed=\"[0-9]*\"/random_seed=$random_seed/" $temp # any series of characters 0-9 are replaced by the random seed
		sed -i "s/output_folder=\"[a-zA-Z0-9]*\"/output_folder=\"$output_folder\"/" $temp #looks for any string in the code mentioned after output folder 
																						  #and writes output to that  will essentially write output to my robot_performance.txt
	fi
		                                                                     
	argos3 -c $temp # executes the argos simulator with the parameters i parse in
done

rm -rf "experiments/$output_folder" # Delete temporary configuration files (no longer needed)
#first input is experiment name i.e room_obstacle, second input is the number of times to run

#INTERESTING NOTES
#(sed is a linux command that finds and replaces things)