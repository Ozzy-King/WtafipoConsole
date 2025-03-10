set /p texte=< cont.txt  
docker cp %texte%:/app/project/build/my_program.uf2 E:\