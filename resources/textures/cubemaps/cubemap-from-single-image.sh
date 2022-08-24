#usage : ./cubemap-from-single-png fichier.png

fileName="$1"  #grass.png
extension="${fileName##*.}"  #png
dirName="${fileName%.*}"  #grass

mkdir "${dirName}"

if [ "${extension}" == "png" ]; then
    extension="/px.png"
    cp ${fileName} "${dirName}/px.png" 
    cp ${fileName} "${dirName}/py.png" 
    cp ${fileName} "${dirName}/pz.png" 
    cp ${fileName} "${dirName}/nx.png" 
    cp ${fileName} "${dirName}/ny.png" 
    cp ${fileName} "${dirName}/nz.png" 
    
    #flipper à l'envers le côté problématique
    convert -flip "${dirName}/px.png" "${dirName}/px.png" 
    
else
    extension="/posx.jpg"
    cp ${fileName} "${dirName}/posx.jpg" 
    cp ${fileName} "${dirName}/posy.jpg" 
    cp ${fileName} "${dirName}/posz.jpg" 
    cp ${fileName} "${dirName}/negx.jpg" 
    cp ${fileName} "${dirName}/negy.jpg" 
    cp ${fileName} "${dirName}/negz.jpg" 

    #flipper à l'envers le côté problématique
    convert -flip "${dirName}/posx.jpg" "${dirName}/posx.jpg" 
fi 

mv "${fileName}" "${dirName}"

echo "Fragmenté avec succès en ${extension}"