echo "var imgInfos = ["
echo "  {},"

for f in $1/*.flif; do 
  flifBaseName=$(basename "$f")
  imgName="${flifBaseName%.*}"
  flifSize=$( stat -c '%s' "$f" )

  echo "  { name: '$imgName',"
  echo "    fileSizes: { flif: $flifSize"

  pngFileName="${f%.*}-i.png"
  if [[ -e "$pngFileName" ]] ; then

    pngFileName="${f%.*}-i.png"

    pngSize=$( stat -c '%s' "$pngFileName" )

    echo "      , png: $pngSize},"

    imgInfo=`pnginfo $pngFileName | head -2 | tail -1`

    [[ $imgInfo =~ (.*)Width:\ ([[:digit:]]+).*Length:\ ([[:digit:]]+) ]] && width=${BASH_REMATCH[2]};height=${BASH_REMATCH[3]}

    echo "    imgSize: { width: $width, height: $height},"
  else
    echo "     }"
  fi


  echo "  },"
done;

echo "];"
