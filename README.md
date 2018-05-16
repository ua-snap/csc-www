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
   git clone https://github.com/ua-snap/csc-www.git
   cd csc-www
   mv ~/Downloads/csc.sql database/
   cd sites/default
   tar -jxvf ~/Downloads/files-csc.bz2
   docker-compose up
   docker exec -i cscwww-drupal drush dis restrict_by_ip securepages
   ```

### Stop Docker containers

The Docker containers can be stopped at any time with the following command:

```bash
docker-compose stop
```

This is useful if you need to start the Docker containers for a different website.

### Start Docker containers

Stopped Docker containers can be started with the following command:

```bash
docker-compose up
```

### Remove Docker containers

If something goes wrong with your Docker containers and you would like to go through the setup instructions again, you will first need to remove your existing Docker containers for the AK CSC website with the following commands.

 ```bash
 docker-compose rm
 ```
