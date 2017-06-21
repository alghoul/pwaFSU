
       subroutine fort_open(NAME)
       CHARACTER*25 NAME
       open(7,file=NAME,status="new")

       return
       end
