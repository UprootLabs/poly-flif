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

    pngSize=$( stat -c '%s' "$pngFileName" )

    echo "      , png: $pngSize"

    imgInfo=`pnginfo $pngFileName | head -2 | tail -1`

    [[ $imgInfo =~ (.*)Width:\ ([[:digit:]]+).*Length:\ ([[:digit:]]+) ]] && width=${BASH_REMATCH[2]};height=${BASH_REMATCH[3]}

  fi

  jpgFileName="${f%.*}-p.jpg"
  if [[ -e "$jpgFileName" ]] ; then

    jpgSize=$( stat -c '%s' "$jpgFileName" )

    echo "      , jpg: $jpgSize"

    if [[ -z imgInfo ]] ; then
      imgInfo=`jpeginfo $pngFileName`

      [[ $imgInfo =~ ([^\s]+)\ ([[:digit:]]+)\ x\ ([[:digit:]]+) ]] && width=${BASH_REMATCH[2]};height=${BASH_REMATCH[3]}

    fi
  fi

  echo "     },"

  if [[ -n $width ]] ; then 
    echo "    imgSize: { width: $width, height: $height},"
  fi

  width=""
  height=""

  echo "  },"
done;

echo "];"
