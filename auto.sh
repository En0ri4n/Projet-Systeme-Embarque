#! /bin/sh

printf "########################\n"
printf "#    COMPIL ARDUINO    #\n"
printf "#      by En0ri4n      #\n"
printf "########################\n"

for i in "$@"                               # Pour chaque arguments présent
do
    case $i in f=*)                         # Si l'argument est sous forme 'f=<something>'
        FOLDER="${i#*=}"                    # Alors on met le nom du dossier donné en paramètre
    shift
    ;;
esac
done

if [ ! -d "$FOLDER" ]; then                 # Si le dossier n'existe pas, alors on exit
    echo "Le dossier $FOLDER n'existe pas"
    exit
elif [ "$FOLDER" != '' ]; then              # Si le nom de dossier n'est pas inexistant, on continue
    printf 'P1'
    cd $FOLDER
    if [ ! -d ".tmp" ]; then                # On vérifie si le dossier .tmp existe et on vide son contenu, sinon on le crée
        mkdir .tmp
    else
        rm -rf .tmp/*
    fi
    printf '.'
    if [ ! -d "build" ]; then               # On vérifie si le dossier build existe et on vide son contenu, sinon on le crée
        mkdir build
    else
        rm -rf build/*
    fi
    printf '. done [env ready]\n'
else                                                            # Sinon, on affiche une erreur et on exit
    echo 'Aucun dossier spécifié, indiquer f=<nom du dossier>'
    exit
fi

printf "P2"
for c in *.cpp; do                                                                # Pour chaque fichier .c trouvé dans le dossier
    [ -f "$c" ] || break
    avr-gcc -g -Os -DF_CPU=16000000UL -mmcu=atmega328p -c $c -o ".tmp/${c%.*}.o"   # On compile le fichier c en fichier objet .obj
    filesC="$filesC $c"                                                         # On l'ajoute à la liste
done
printf '. done [.o ready]\n'

printf 'P3'
avr-gcc -g -DF_CPU=16000000UL -mmcu=atmega328p -o build/$FOLDER $filesC 2> /dev/null  # Compile tous les fichiers en un seul fichier elf
printf '. done [links]\n'

printf 'P4'
avr-objcopy -O ihex -R .eeprom build/$FOLDER build/$FOLDER.hex                     # On récupère seulement le fichier hex nécessaire 
printf '. done [.hex]\n'

printf "Voulez vous commencer le téléversement ? [Y/N]: "
read -r TELEV                                                                      # On demande si l'utilisateur veut téléverser
if [ "$TELEV" = "N" ]; then
    printf "Ciao Amigos\n"
    exit
else
    printf "Démarrage du téléversement \n"
    avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 9600 -U flash:w:build/$FOLDER.hex # Versement du programm sur l'Arduino
fi

exit
