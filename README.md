# AK CSC Drupal

## Deploying updates to `nyx.snap.uaf.edu`

To pull the latest code from the `master` branch of this repository and recompile the Sass stylesheets, do this on your local machine:

`ssh -t username@nyx.snap.uaf.edu update_drupal.sh`

## Local development instance with Docker

### Setup Docker containers

1. Install [Docker](https://www.docker.com/) if you have not already.

1. Grab AK CSC files and database dump from https://csc.alaska.edu/sites/all/gimme.php, save them to your ~/Downloads folder.

1. Clone this repository and extract the Drupal files into their proper location:

   ```bash
   mkdir -p ~/docker
   cd ~/docker
   git clone https://github.com/ua-snap/csc-www.git
   cd csc-www/sites/default
   tar -jxvf ~/Downloads/files-csc.bz2
   ```

1. Download Drupal settings file that reads MySQL host and root password from Docker environment variables.

   ```bash
   curl -O 'https://raw.githubusercontent.com/ua-snap/docker-drupal-settings/master/settings.php'
   ```

1. Set up persistent MySQL database container. You may need to wait 10-20 seconds after this command returns before you can connect to the MySQL server in the next step.

   ```bash
   docker run --name csc-mysql -e MYSQL_ROOT_PASSWORD=root -d mysql:latest
   ```

1. Spawn temporary container that links to MySQL container and creates database.

   ```bash
   docker run -it --link csc-mysql:mysql --rm mysql sh -c 'exec mysql \-h "$MYSQL_PORT_3306_TCP_ADDR" -P "$MYSQL_PORT_3306_TCP_PORT" -uroot -p"$MYSQL_ENV_MYSQL_ROOT_PASSWORD" -e "CREATE DATABASE drupal7;"'
   ```

1. Spawn temporary container that links to MySQL container and imports database dump.

   ```bash
   docker run -i --link csc-mysql:mysql --rm mysql sh -c 'exec mysql \-h "$MYSQL_PORT_3306_TCP_ADDR" -P "$MYSQL_PORT_3306_TCP_PORT" -uroot -p"$MYSQL_ENV_MYSQL_ROOT_PASSWORD" drupal7' < ~/Downloads/csc.sql
   ```

1. Set up persistent Drupal container that links to MySQL container.

   ```bash
   docker run --name csc-drupal -p 8080:80 --link csc-mysql:mysql -v ~/docker/csc-www:/var/www/html -d drupal:7
   ```

1. Disable the Secure Pages and Restrict by IP modules with Drush so you can log in to your development instance:

   ```bash
   docker exec -i csc-drupal sh -c 'apt-get update && apt-get install -y drush'
   docker exec -i csc-drupal drush dis restrict_by_ip securepages
   ```

### Stop Docker containers

The Docker containers can be stopped at any time with the following command:

```bash
docker stop csc-drupal csc-mysql
```

This is useful if you need to start the Docker containers for a different website.

### Start Docker containers

Stopped Docker containers can be started with the following command:

```bash
docker start csc-drupal csc-mysql
```

### List Docker containers

The following command will list all of the Docker containers on your machine, both running and not running:

```bash
docker ps -a
```

### Remove Docker containers

If something goes wrong with your Docker containers and you would like to go through the setup instructions again, you will first need to remove your existing Docker containers for the AK CSC website with the following commands.

1. Stop the Drupal and MySQL containers:

   ```bash
   docker stop csc-drupal csc-mysql
   ```

1. Remove the Drupal and MySQL containers:

   ```bash
   docker rm csc-drupal csc-mysql
   ```
