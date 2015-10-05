echo "var imgInfos = ["
echo "  {},"

for f in $1/*.flif; do 
  flifBaseName=$(basename "$f")
  imgName="${flifBaseName%.*}"
  pngFileName="${f%.*}-i.png"

  flifSize=$( stat -c '%s' "$f" )
  pngSize=$( stat -c '%s' "$pngFileName" )

  imgInfo=`pnginfo $pngFileName | head -2 | tail -1`

  [[ $imgInfo =~ (.*)Width:\ ([[:digit:]]+).*Length:\ ([[:digit:]]+) ]] && width=${BASH_REMATCH[2]};height=${BASH_REMATCH[3]}


  echo "  { name: '$imgName',"
  echo "    imgSize: { width: $width, height: $height},"
  echo "    fileSizes: { flif: $flifSize, png: $pngSize}"
  echo "  },"
done;

echo "];"
