if [ "$#" -ne 1 ]; then
  echo "Usage ./update_docker.sh [docker-repo-name]"
  exit 1
fi

./gradlew :core:actionProxy:distDocker :sdk:docker:distDocker
docker tag whisk/dockerskeleton $1
docker push $1
