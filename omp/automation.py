import subprocess

#configuration
batch_file = "mp.sbatch"
src_files = ["./mp1.c" ,"./mp2.c"]
threads = [2, 4, 6, 8]

for src in src_files:
   exe = src.replace(".c", "")
   for num_threads in threads:
       job_name = f"{exe} - {num_threads}t"
       log_name = f"{exe} - {num_threads}t_%j.out"
       
       cmd = [
           "sbatch",
           "-J", job_name,
           "-o", f"logs/{log_name}",
           "--export", f"ALL,EXE={exe},OMP_T={num_threads}",
           batch_file
       ]

       print("Submitting:", " ".join(cmd))
       subprocess.run(cmd, check=True)
    