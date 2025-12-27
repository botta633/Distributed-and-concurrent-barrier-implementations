import subprocess

#configuration
batch_file = "mpi.sbatch"
src_files = ["./mpi1.c" ,"./mpi2.c"]
nodes = [2, 4, 6, 8]

for src in src_files:
   exe = src.replace(".c", "")
   for num_nodes in nodes:
       job_name = f"{exe} - {num_nodes}t"
       log_name = f"{exe} - {num_nodes}t_%j.out"
       
       cmd = [
           "sbatch",
           "-J", job_name,
           "-o", f"logs/{log_name}",
           "--export", f"ALL,EXE={exe},NODES={num_nodes}",
           batch_file
       ]

       print("Submitting:", " ".join(cmd))
       subprocess.run(cmd, check=True)
    