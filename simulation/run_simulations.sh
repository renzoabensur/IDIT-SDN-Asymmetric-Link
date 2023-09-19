#!/bin/bash
#set -e
set -x
#set -v

# Simulation set configuration
MIN_ITER=1
MAX_ITER=3
COOJA_INSTANCES=3 #max simulations running in parallel
COOJA_CURRENT_INSTANCE=1

# nodes_v=(36 100)
nodes_v=(36)
# topologies=(a1_1 a2_1 a3_1 a1_3 a2_3 a3_3)
topologies=(a1_3)
# SIM_TIME_MS=1800000 # 30 minutes
# SIM_TIME_MS=10800000  # 3 h
SIM_TIME_MS=18000000 # 5 h
# SIM_TIME_MS=36000000 # 10 h


# the follwing variables are defined in path_simulation.sh:
# contiki_dir, cooja_dir, simulation_files_dir, simulation_output_dir,
# build_dir, controller_dir, controller_build_dir, QMAKE, CONTIKI
source path_simulation.sh

# list of process ids of currently running cooja simulators
pidJava=()
# list of process ids of currently running controllers
pidController=()
# list of process simlation logs (to run sdn_get_statistics_preproc.py on)
simulationLogs=()

run_cooja() {
# $1 => path to cooja .csc file
# $2 => where the cooja log will be saved
# $2 => where the controller log will be saved
# $4 => number employed to choose a different cooja directory for
#				each parallel simulation
	# (
		cd "${contiki_dir}${4}${cooja_dir}/dist"
		java -mx12120m -jar cooja.jar -nogui="$1" > $2 2>&1 &

		# append the pid of the last process (i.e. $!) to the list
		pidJava=(${pidJava[@]} $!)
		echo ${pidJava[@]}

		# wait until the cooja log exists
		while [ ! -f $2 ]; do
				sleep 0.5s
				echo "Waiting log"
		done

		# wait until the serial socket is available
		while [ `grep -c "Listening on port" $2` -ne 1 ]; do
						sleep 0.5s
						echo "Waiting for serial socket to become available"
		done

		# runnig the controller
		"$controller_build_dir"/controller-pc $nnodes --platform offscreen > $3 2>&1 &
		# appends the pid of the last process (i.e. $!) to the list
		pidController=(${pidController[@]} $!)
		cd -
	# )
}

# echo "" > makelog.log

mkdir -p $simulation_output_dir

if [ $COOJA_INSTANCES -gt $(($MAX_ITER - $MIN_ITER + 1)) ]; then
	echo COOJA_INSTANCES has to be larger than the iteration range defined by MAX_ITER and MIN_ITER
	exit
fi

for nnodes in "${nodes_v[@]}"; do
	for topo in "${topologies[@]}"; do
		# for nd in "${nd_possibilities[@]}"; do
			# for datarate in "${datarates[@]}"; do
				for iter in `seq $MIN_ITER $MAX_ITER`; do
					# s - schedule (CTK - contiki default, IND - individual slots for each node)
					# n - number of nodes
					# d - data traffic type
					# l - lambda (data rate = 1/packet interval)
					# i - iteration (experimetn number)

					port=$((60000+$iter));

					cooja_file="$simulation_files_dir"/"ITSDN_n"${nnodes}"_s1_"${topo}"_GRID".csc
					#cooja_file="$simulation_files_dir"/${topo}_${nnodes}_2.csc
					cooja_motes_out_file="$simulation_output_dir"/"ITSDN_n"$nnodes"_top_"$topo"_i"$iter'.txt'
					cooja_log_file="$simulation_output_dir"/"ITSDN_log_n"$nnodes"_top_"$topo"_i"$iter'.txt'
					controller_out_file="$simulation_output_dir"/"controller_ITSDN_n"$nnodes"_top_"$topo"_i"$iter'.txt'
					simulationLogs=(${simulationLogs[@]} ${cooja_motes_out_file})

					echo Cooja simulation file: $cooja_file
					echo Cooja logfile name: $cooja_motes_out_file
					echo Controller logfile name: $controller_out_file

					# Change the COOJA simuation file to save simulation log with a different name
					sed -i.bak "s,my_output =.*,my_output = new FileWriter(\"$cooja_motes_out_file\");," ${cooja_file}

					# change csc file serial server port accordingly
					sed -i.bak "s,<port>60.*,<port>${port}</port>," ${cooja_file}
					# setting simulation timeout
					sed -i.bak "s,TIMEOUT.*,TIMEOUT($SIM_TIME_MS);," ${cooja_file}

					# Clean current binaries
					(
						cd "$build_dir"

						#if [ "$nd" == "CL" ]; then
						#	./change_to_collect.sh
						#elif [ "$nd" == "NV-NV" ]; then
						#	./change_to_NV-NV.sh
						#elif [ "$nd" == "IM-NV" ]; then
						#	./change_to_IM-NV.sh
						#elif [ "$nd" == "NV-SC" ]; then
						#	./change_to_NV-SC.sh
						#elif [ "$nd" == "IM-SC" ]; then
						#	./change_to_IM-SC.sh
						# elif [ "$nd" == "BL" ]; then
						#else
						#	echo "there is no such ND algorithm"
						#	continue
						#fi

						make TARGET=sky clean -f Makefile_enabled_node
						make TARGET=sky -f Makefile_enabled_node sink-node

						make TARGET=sky clean -f Makefile_enabled_node
						make TARGET=sky -f Makefile_enabled_node enabled-node

						make TARGET=sky clean -f Makefile_controller_node
						make TARGET=sky -f Makefile_controller_node

						make TARGET=sky clean -f Makefile_enabled_node
						make TARGET=sky -f Makefile_enabled_node management-sink
						
						make TARGET=sky clean -f Makefile_enabled_node
						make TARGET=sky -f Makefile_enabled_node attack-one

						# make clean -f Makefile_enabled_node
						# make -f Makefile_enabled_node attack-two

						# make clean -f Makefile_intrusion_node
						# make -f Makefile_intrusion_node attack-three

					)

					#controller recompilation
					(
						cd $controller_build_dir
						# changing serial server expected port
						sed -i.bak "s,<number>60.*,<number>${port}</number>," ../controller-pc/mainwindow.ui
						$QMAKE ../controller-pc/controller-pc.pro
						$MAKE clean
						$MAKE
					)

					run_cooja `readlink -f ${cooja_file}` "$cooja_log_file" "$controller_out_file" $COOJA_CURRENT_INSTANCE
					((COOJA_CURRENT_INSTANCE++))

					# reset csc file parameters
					sed -i.bak "s,my_output =.*,my_output = new FileWriter(\"/dev/null\");," ${cooja_file}
					sed -i.bak "s,<port>60.*,<port>60001</port>," ${cooja_file}
					sed -i.bak "s,TIMEOUT.*,TIMEOUT(1800000);," ${cooja_file}

					if [ ${#pidJava[@]} -eq $COOJA_INSTANCES ]; then
						# Waiting simulations to finish
						wait ${pidJava[@]}

						# python sdn_get_statistics_preproc.py  $cooja_motes_out_file &
						#for simulationLog in "${simulationLogs[@]}"; do
						#	python sdn_get_statistics_preproc.py  $simulationLog &
						#done

						# Killing controllers
						kill ${pidController[@]}

						# reseting pid lists
						pidJava=()
						pidController=()
						simulationLogs=()
						COOJA_CURRENT_INSTANCE=1
					fi

				done
			# done
		# done
	done
done

# wait for any remaining simulations
wait ${pidJava[@]}
# python sdn_get_statistics_preproc.py  $cooja_motes_out_file &
#for simulationLog in "${simulationLogs[@]}"; do
#	python sdn_get_statistics_preproc.py  $simulationLog &
#done
# Killing controllers
kill ${pidController[@]}

# controller default port
(
	cd $controller_build_dir
	# changing serial server expected port
	sed -i.bak "s,<number>60.*,<number>60001</number>," ../controller-pc/mainwindow.ui
	$QMAKE ../controller-pc/controller-pc.pro
	$MAKE clean
	$MAKE
)
echo END
