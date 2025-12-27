import subprocess

#configuration
batch_file = "combined.sbatch"
src_files = ["./combined"]
nodes = [2, 4, 6, 8]
threads = [2, 4, 6, 8]

for src in src_files:
   exe = src.replace(".c", "")
   for num_nodes in nodes:
    for num_thread in threads:
       job_name = f"{exe} - {num_nodes}t"
       log_name = f"{exe} - {num_nodes}t_%j.out"
       
       cmd = [
           "sbatch",
           "-J", job_name,
           "-o", f"logs/{log_name}",
           "--export", f"ALL,EXE={exe},NODES={num_nodes},OMP_T={num_thread}",
           batch_file
       ]

       print("Submitting:", " ".join(cmd))
       subprocess.run(cmd, check=True)
    