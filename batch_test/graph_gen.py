# coding=utf-8

import os
import sys
from random import randint

nb = int(sys.argv[1])
folder = sys.argv[2]

MIN_READ = 4
MAX_READ = 6
MAX_SIZE = 20
MAX_SEED = 10
MAX_VAR = 1
MIN_SEEDS = 2

if(not os.path.isdir(folder)):
    os.mkdir(folder)

for n in range(nb):
    out = open(os.path.join(folder, "graph_" + str(n) + ".edges"), "w")

    nb_reads = randint(MIN_READ, MAX_READ)
    nb_source = randint(1, nb_reads - round(nb_reads / 2))
    read_ids = [i for i in range(nb_reads)]
    edge_format = "read_{}\t" + \
        str(MAX_SIZE) + "\tread_{}\t" + str(MAX_SIZE) + "\t{}\t1"

    out.write("Number of reads: " + str(nb_reads))
    out.write(" Number of sources: " + str(nb_source) + "\n")
    # for each source
    for r in read_ids[:nb_source]:
        base_set = set([randint(0, MAX_SIZE) for _ in range(MAX_SEED)])
        base_position = sorted(list(base_set))

        for t in read_ids[nb_source:]:
            r_seeds = []
            t_seeds = []

            if(randint(0, 2) != 0):  # 1/2 chance
                for r_pos in base_position:
                    if(randint(0, 2) != 0):  # 1/2 chance
                        t_pos = r_pos + (randint(0, MAX_VAR * 2) - MAX_VAR)
                        t_pos = abs(t_pos)
                        t_pos = t_pos+1 if len(t_seeds)>0 and t_pos == t_seeds[-1] else t_pos

                        r_seeds.append(r_pos)
                        t_seeds.append(t_pos)

            # CHANGE HERE TO ALLOW REVERSE
            # reverse = randint(0, 1)
            reverse = 0

            line = edge_format.format(r, t, reverse)
            if(reverse == 1):
                t_seeds = t_seeds[::-1]
            for k in range(len(r_seeds)):
                line += "\t" + str(r_seeds[k]) + "," + str(t_seeds[k])
            if(len(r_seeds) >= MIN_SEEDS):
                out.write(line + "\n")
    out.close()
