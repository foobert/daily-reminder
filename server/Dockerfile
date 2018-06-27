FROM node:alpine
COPY package*.json /usr/src/app/
WORKDIR /usr/src/app
RUN npm install --production
COPY index.js /usr/src/app/
EXPOSE 3000
CMD npm start
