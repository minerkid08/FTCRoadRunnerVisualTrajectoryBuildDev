#include <NodeGrid.h>
#include <glm/gtc/matrix_transform.hpp>

NodeGrid::NodeGrid(Shader* _shader) : circleTex("circle.png"), arrowTex("arrow.png"){
	nodes = new PathNode[maxNodes]();
	nodeCount = 0;
	shader = _shader;
}

NodeGrid::~NodeGrid(){
	delete nodes;
}
void NodeGrid::update(Renderer& renderer, int mouseX, int mouseY, int windowSize){
	if(selected >= nodeCount){
		selected = nodeCount - 1;
	}
	if(layer < -1){
		layer = -1;
	}
	if(nodeCount > 0){
		if(nodeCount > 1){
			for(int i = 0; i < nodeCount - 1; i++){
				PathNode* node1 = (nodes + i);
				PathNode* node2 = (nodes + i + 1);
				glm::vec2 pos1 = {node1->pos.x / 72, node1->pos.y / 72};
				glm::vec2 pos2 = {node2->pos.x / 72, node2->pos.y / 72};
				glm::vec2 dif = pos2 - pos1;
				glm::vec2 dif2 = glm::normalize(dif);
				dif2 = {dif2.x / 40, dif2.y / 40};
				glm::vec4 verts[4];
				verts[0] = {dif2.y + pos1.x, -dif2.x + pos1.y, 0, 1};
				verts[1] = {-dif2.y + pos1.x, dif2.x + pos1.y, 0, 1};
				verts[2] = {dif2.y + pos1.x + dif.x, -dif2.x + dif.y + pos1.y, 0, 1};
				verts[3] = {-dif2.y + pos1.x + dif.x, dif2.x + dif.y + pos1.y, 0, 1};
				renderer.draw(verts, &arrowTex, shader, {0.5f, 0.5f, 0.5f, 1});
			}
		}
		for(int h = 0; h < 2; h++){
			if(layer == -1 && h == 1){
				break;
			}
			for(int i = 0; i < nodeCount + 1; i++){
				if(i == selected || selected >= nodeCount){
					continue;
				}
				PathNode* node;
				if(i < nodeCount){
					node = (nodes + i);
				}else{
					node = (nodes + selected);
				}
				if(node->layer < 0){
					node->layer = 0;
				}
				if(!(node->layer == layer || layer < 0) && h == 1){
					continue;
				}
				if(node->layer == layer && h == 0){
					continue;
				}
				glm::mat4 mat = glm::rotate(
					glm::mat4(1), 
					glm::radians(node->rot), glm::vec3(0, 0, 1)
				);
				glm::vec4 verts[4] = {
					glm::vec4(+0.04, +0.04, 0, 1) * mat,
					glm::vec4(+0.04, -0.04, 0, 1) * mat,
					glm::vec4(-0.04, +0.04, 0, 1) * mat,
					glm::vec4(-0.04, -0.04, 0, 1) * mat,
				};
				for(int j = 0; j < 4; j++){
					verts[j] = {node->pos.x / 72 + verts[j].x, node->pos.y / 72 + verts[j].y, 0, 1};
				}
				glm::vec4 tint(0,0,0,1);
				bool a = false;
				if((selected == 0 && i == nodeCount) || i == 0){
					tint.g = 1;
				}
				if(i == nodeCount){
					tint.r = 1;
				}
				if(node->turnAfterMove){
					tint.b = 1;
					a = true;
				}
				if(tint == glm::vec4(0, 0, 0, 1)){
					tint = {1, 1, 1, 1};
				}
				if(tint == glm::vec4(1, 1, 1, 1) && a){
					tint = {-1, -1, -1, 1};
				}
				if(h == 0 && layer != -1){
					tint.a = 0.5;
				}
				renderer.draw(verts, &circleTex, shader, tint);
			}
		}
	}
	if(clickMode == 0){
		float x = (mouseX - (float)(windowSize/2)) / (windowSize/2) * 72;
		float y = (mouseY - (float)(windowSize/2)) / (windowSize/2) * 72;

		x = round(x / 6.0f) * 6;
		y = round(y / 6.0f) * -6;

		x /= 72;
		y /= 72;

		glm::vec4 verts[4] = {
			glm::vec4(+0.04, +0.04, 0, 1),
			glm::vec4(+0.04, -0.04, 0, 1),
			glm::vec4(-0.04, +0.04, 0, 1),
			glm::vec4(-0.04, -0.04, 0, 1)
		};
		for(int j = 0; j < 4; j++){
			verts[j] = {x + verts[j].x, y + verts[j].y, 0, 1};
		}
		renderer.draw(verts, &circleTex, shader, {1, 1, 1, 0.25});
	}
}

void NodeGrid::mouseClick(int mouseX, int mouseY, int windowSize){
	float x = (mouseX - (float)(windowSize/2)) / (windowSize/2) * 72;
	float y = (mouseY - (float)(windowSize/2)) / (windowSize/2) * 72;
	x = round(x / 6.0f) * 6;
	y = round(y / 6.0f) * -6;
	if(clickMode == 0){
		addNode({x, y});
	}else if(clickMode == 1){
		float closestDist = 100.0f;
		int closestInd = -1;
		for(int i = 0; i < nodeCount; i++){
			PathNode* node = (nodes + i);
			if(node->layer == layer || layer == -1){
				float dist = glm::distance(glm::vec2(x, y), glm::vec2(node->pos.x, node->pos.y));
				if(dist < closestDist){
					closestDist = dist;
					closestInd = i;
				}
			}
		}
		if(closestDist < 12){
			selected = closestInd;
		}
	}
}

void NodeGrid::addNode(glm::vec2 pos){
	if(nodeCount >= maxNodes)
		return;
	(nodes + nodeCount)->pos = pos;
	(nodes + nodeCount)->rot = 0;
	(nodes + nodeCount)->turnAfterMove = false;
	(nodes + nodeCount)->marker = false;
	(nodes + nodeCount)->layer = (layer == -1) ? 0 : layer;
	selected = nodeCount;
	nodeCount++;
}

void NodeGrid::removeNode(int ind){
	for(int i = ind; i < nodeCount - 1; i++){
		PathNode* node = (nodes + i);
		node->pos = (nodes + i + 1)->pos;
		node->rot = (nodes + i + 1)->rot;
	}
	nodeCount--;
}

void NodeGrid::moveUp(int ind){
	if(ind + 1 < nodeCount){
		PathNode node = *(nodes + ind + 1);
		(nodes + ind + 1)->pos = (nodes + ind)->pos;
		(nodes + ind + 1)->rot = (nodes + ind)->rot;
		(nodes + ind + 1)->turnAfterMove = (nodes + ind)->turnAfterMove;
		(nodes + ind + 1)->marker = (nodes + ind)->marker;
		(nodes + ind + 1)->layer = (nodes + ind)->layer;

		*(nodes + ind) = node;
		selected++;
	}
}

void NodeGrid::moveDown(int ind){
	if(ind - 1 > -1){
		PathNode node = *(nodes + ind - 1);
		(nodes + ind - 1)->pos = (nodes + ind)->pos;
		(nodes + ind - 1)->rot = (nodes + ind)->rot;
		(nodes + ind - 1)->turnAfterMove = (nodes + ind)->turnAfterMove;
		(nodes + ind - 1)->marker = (nodes + ind)->marker;
		(nodes + ind - 1)->layer = (nodes + ind)->layer;

		*(nodes + ind) = node;
		selected--;
	}
}

void NodeGrid::flipVert(){
	for(int i = 0; i < nodeCount; i++){
		(nodes + i)->pos.y *= -1;
	}
}

void NodeGrid::flipHoriz(){
	for(int i = 0; i < nodeCount; i++){
		(nodes + i)->pos.x *= -1;
	}
}