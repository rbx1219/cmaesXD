type="FHH"
chromosome_length=10
seed=$(($(od -vAn -N4 -tu4 < /dev/urandom) % 671262642))
population_size=250
probability_crossover=0.975
split_rate=0.45
decrease_rate=0.988
max_model=5
tournament_size=8
Bin_num=18
learn_MPM="on"
stop_criteria="max_generations"
stop_criteria_argument=999
report_err="off"
report_pop="off"
report_string="off"
report_fitness="off"
report_MPM="off"
up_bound=25.000000
low_bound=-25.000000
ans=260.000000
num_fun=1


for ((k=0; k<1; k=k+1))
do
#decay_rate=`echo "$decay_rate+0.05" | bc -l`
for ((j=0; j<1; j=j+1))
do	#do1
	#tournament_size=$(($tournament_size + 0.05))
	#virtual_points_rate=`echo "$virtual_points_rate+0.05" | bc -l`
for ((i=26; i<26; i=i+1))
do	#do2
	case $i in
	1|3|5|6|14) up_bound=100 low_bound=-100;; 2|4) up_bound=500 low_bound=-500;;
	7) up_bound=600 low_bound=-600;; 8) up_bound=32.768 low_bound=-32.768;;
	11) up_bound=0.5 low_bound=-0.5;; 13) up_bound=2.048 low_bound=-2.048;;
	26|27) up_bound=80 low_bound=0;;
	*) up_bound=5.12 low_bound=-5.12;;	#*)=9, 10, 12, 15-25
	esac
	
	case $i in
	1|2|3|4|26|27) ans=-450.000000;; 5) ans=-310.000000;; 6) ans=390.000000;; 7) ans=-180.000000;; 
	8) ans=-140.000000;; 9|10) ans=-330.000000;; 11) ans=90.000000;;
	12) ans=-460.000000;; 13) ans=-130.000000;; 14) ans=-300.000000;; 15|16|17) ans=120.000000;;
	18|19|20) ans=10.000000;; 21|22|23) ans=360.000000;; 24|25) ans=260.000000;;
	*) echo "something wrong!!!";;
	esac
	
count=0
while [[ $count -lt 250 ]] 
do	#do3
count=$(($count + 1))
seed=$(($(od -vAn -N4 -tu4 < /dev/urandom) % 671262642))
text="#\n\
# Sample parameter file.\n\
# Don't change the order of the lines in this file.\n\
#\n\
BEGIN\n\
chromosome_length              $chromosome_length\n\
seed                           $seed\n\
population_size                $population_size\n\
probability_crossover          $probability_crossover\n\
split_rate                     $split_rate\n\
decrease_rate                  $decrease_rate\n\
max_model                      $max_model\n\
tournament_size                $tournament_size\n\
Bin_num			       $Bin_num\n\
learn_MPM                      $learn_MPM\n\
stop_criteria                  $stop_criteria\n\
stop_criteria_argument         $stop_criteria_argument\n\
#\n\
# reporting flags\n\
#\n\
report_err                     $report_err\n\
report_pop                     $report_pop\n\
report_string                  $report_string\n\
report_fitness                 $report_fitness\n\
report_MPM                     $report_MPM\n\
up_bound                       $up_bound\n\
low_bound                      $low_bound\n\
ans                            $ans\n\
num_fun                        $i\n\
END"

destiny="./inputfileBFC_$type fun$i max generation$stop_criteria_argument.txt"
echo -e "$text" > $destiny

./rECGA_$type "inputfileBFC_$type fun$i max generation$stop_criteria_argument.txt" "outputfileBFC_$type detail_fun$i.txt" "outputfileBFC_$type short.txt"

input_filename="./outputfileBFC_$type short.txt"
target_filename="./BFC_$type fun$i max generation$stop_criteria_argument.txt"
exec < $input_filename
while read line
do	#do4
	if [[ $count == 1 ]]; then
		echo -e "$line" > "$target_filename"
	else
		echo -e "$line" >> "$target_filename"
	fi
done	#done4
done	#done3	
	echo -e "END" >> "$target_filename"
done	#done2
statical_filename="./$type tournament size=$tournament_size Bin_num=$Bin_num pcross=$probability_crossover mini=1e-5.txt"
for ((i=1; i<26; i=i+1))
do
target_filename="./BFC_$type fun$i max generation$stop_criteria_argument.txt"
./read_statis "$target_filename" "output_temp1"
exec < "output_temp1"
while read line
do
		echo -e "$line" >> "$statical_filename"
done
done	
done	#done1
done	#done0


