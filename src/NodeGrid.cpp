#include <NodeGrid.h>
#include <glm/gtc/matrix_transform.hpp>

void NodeGrid::drawRotatedArrow(Renderer& renderer, glm::vec2 pos, float rot, glm::vec4 tint){
	glm::mat4 mat = glm::rotate(
		glm::mat4(1),
		glm::radians(rot), glm::vec3(0, 0, 1)
	);
	glm::vec4 verts[4] = {
		glm::vec4(+0.04, +0.04, 0, 1) * mat,
		glm::vec4(+0.04, -0.04, 0, 1) * mat,
		glm::vec4(-0.04, +0.04, 0, 1) * mat,
		glm::vec4(-0.04, -0.04, 0, 1) * mat,
	};
	pos.y /= 72;
	pos.x /= 72;
	for(int j = 0; j < 4; j++){
		verts[j] = {pos.x + verts[j].x, pos.y + verts[j].y, 0, 1};
	}
	renderer.draw(verts, &arrowSquareTex, shader, tint);
}

NodeGrid::NodeGrid(Shader* _shader) : circleTex("circle.png"), arrowTex("arrow.png"), arrowSquareTex("arrowSquare.png"), nodes(maxNodes), segs(maxSegs){
	shader = _shader;
}

NodeGrid::~NodeGrid(){}

void NodeGrid::update(Renderer& renderer, int mouseX, int mouseY, int windowSize, int _mods){
	mods = _mods;
	if(selected.ind >= nodes.count){
		selected.ind = nodes.count - 1;
	}
	if(layer < -1){
		layer = -1;
	}
	if(nodes.count > 0){
		if(nodes.count > 1){
			for(int i = 0; i < segs.count; i++){
				PathNode* node1 = nodes.get(segs.get(i)->startNode);
				PathNode* node2 = nodes.get(segs.get(i)->endNode);
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
				renderer.draw(verts, &arrowTex, shader, {selected.type == TypeSegment && selected.ind == i ? 1.0f : 0.5f, 0.5f, 0.5f, (segs.get(i)->layer == layer || layer == -1) ? 1 : 0.5f});
			}
		}
		for(int h = 0; h < 2; h++){
			if((layer == -1 || selected.ind == -1) && h == 1){
				break;
			}
			for(int i = 0; i < nodes.count + 1; i++){
				if(i == selected.ind || selected.ind >= nodes.count || (selected.ind == -1 && i == nodes.count)){
					continue;
				}
				PathNode* node;
				if(i < nodes.count){
					node = nodes.get(i);
				}else{
					node = nodes.get(selected.ind);
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
				glm::vec2 pos = node->pos;
				pos.x /= 72;
				pos.y /= 72;
				for(int j = 0; j < 4; j++){
					verts[j] = {pos.x + verts[j].x, pos.y + verts[j].y, 0, 1};
				}
				glm::vec4 tint(1, 1, 1, 1);
				bool a = false;
				if(i == nodes.count && selected.type == TypeNode){
					tint = {1, 0, 0, 1};
				}

				if(h == 0 && layer != -1){
					tint.a = 0.5;
				}
				renderer.draw(verts, &circleTex, shader, tint);
				tint = {0, 0.5f, 1, 1};
				if(h == 0 && layer != -1){
					tint.a = 0.5;
				}
				drawRotatedArrow(renderer, node->pos, node->heading, tint);
			}
		}
	}
	if(mods == 1){
		float x = (mouseX - (float)(windowSize/2)) / (windowSize/2) * 72;
		float y = (mouseY - (float)(windowSize/2)) / (windowSize/2) * 72;
		
		if(gridSnap){
			x = round(x / 6.0f) * 6;
			y = round(y / 6.0f) * -6;
		}else{
			y *= -1;
		}

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

void NodeGrid::mouseClick(int mouseX, int mouseY, int windowSize, int mods){
	float x = (mouseX - (float)(windowSize/2)) / (windowSize/2) * 72;
	float y = (mouseY - (float)(windowSize/2)) / (windowSize/2) * 72;
	if(gridSnap){
		x = round(x / 6.0f) * 6;
		y = round(y / 6.0f) * -6;
	}else{
		y *= -1;
	}
	if(mods == 1){
		nodes.add()->pos = {x, y};
		resetNode(nodes.count - 1);
	}else{
		float closestDist = 100.0f;
		int closestInd = -1;
		int closestType = -1;
		for(int i = 0; i < nodes.count; i++){
			PathNode* node = nodes.get(i);
			if(node->layer == layer || layer == -1){
				float dist = glm::distance(glm::vec2(x, y), glm::vec2(node->pos.x, node->pos.y));
				if(dist < closestDist){
					closestDist = dist;
					closestInd = i;
					closestType = TypeNode;
				}
			}
		}
		for(int i = 0; i < segs.count; i++){
			PathSegment* seg = segs.get(i);
			if(seg->layer == layer || layer == -1){
				PathNode* start = nodes.get(seg->startNode);
				PathNode* end = nodes.get(seg->endNode);
				glm::vec2 pos = {(start->pos.x + end->pos.x) / 2.0f, (start->pos.y + end->pos.y) / 2.0f};
				float dist = glm::distance(glm::vec2(x, y), glm::vec2(pos.x, pos.y));
				if(dist < closestDist){
					closestDist = dist;
					closestInd = i;
					closestType = TypeSegment;
				}
			}
		}
		if(closestDist < 12){
			if(mods == 0){
				selected.ind = closestInd;
				selected.type = closestType;
			}else if(selected.ind >= 0 && selected.type == TypeNode && closestType == TypeNode){
				PathSegment* seg = segs.add();
				seg->startNode = selected.ind;
				seg->endNode = closestInd;
				seg->headingMode = 0;
				seg->pathType = 0;
				seg->layer = __max(layer, 0);
				selected.ind = closestInd;
				selected.type = TypeNode;
			}
		}
	}
}

void NodeGrid::resetNode(int ind){
	PathNode* node = nodes.get(ind);
	node->rot = 0;
	node->layer = __max(layer, 0);
	node->layer = (layer == -1) ? 0 : layer;
	for(NodePart* part : node->parts){
		delete part;
	}
	node->parts.clear();
}

void NodeGrid::flipVert(){
	nodes.foreach([](int ind, PathNode* node){
		node->pos.y *= -1;
		node->rot += 90;
		node->rot *= -1;
		node->rot -= 90;
	});
}

void NodeGrid::flipHoriz(){
	nodes.foreach([](int ind, PathNode* node){
		node->pos.x *= -1;
		node->rot *= -1;
	});
}

void NodeGrid::reset(){
	nodes.foreach([](int i, PathNode* node){
		for(NodePart* part : node->parts){
			delete part;
		}
		node->parts.resize(0);
	});
	nodes.count = 0;
	segs.foreach([](int i, PathSegment* seg){
		for(SegPart* part : seg->parts){
			delete part;
		}
		seg->parts.resize(0);
	});
	segs.count = 0;
}